<?php

namespace App\Http\Controllers;

use App\Models\Product;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Validator;
use App\Models\User;
use App\Models\Mantel;
use App\Models\Comida;
use App\Models\Plato;
use App\Models\Alimento;
use App\Models\TipoAlimento;
use App\Models\Grupo;
use App\Models\Barcode;

class ComidaController extends Controller {
    
    // Guarda una comida o grupo de comidas en la base de datos
    public function store(Request $request) {
        // Validación de que se envian desde el mantel todos los datos json completos con sus nombres de claves correctas (con campo fecha/hora en formato Unix)
        $validator = Validator::make($request->input(), [
            'comidas'=>'required', 
            'comidas.*.fecha'=>'required|regex:/^[0-9]{1,10}/', 
            'comidas.*.platos'=>'required',
            'comidas.*.platos.*.alimentos'=>'required', 
            'comidas.*.platos.*.alimentos.*.grupo'=>'required|int', 
            'comidas.*.platos.*.alimentos.*.peso'=>'required|regex:/^[0-9]+([.][0-9])+/'
        ]);
        
        if ($validator->fails()) 
        {
            return response()->json([
                'success' => false,
                'errors' => $validator->errors()->all()
            ], 400);
        } 
        else 
        {
            // $datos es un objeto cuyas propiedades son mac y (comidas que es un array de comidas)
            $datos = json_decode($request->getContent());
            $mantel = Mantel::where('mac', $datos->mac)->first();
            if (empty($mantel)) 
            {
                return response()->json([
                    'success' => false,
                    'message' => 'MAC no encontrada',
                ], 401);
            } 
            else 
            {
                $user_id = $mantel->user->id; // Obtener el id del usuario al que pertenece el mantel

                foreach ($datos->comidas as $comida)  // $datos->comidas es un array de comidas. $comida es un objeto cuya propiedad es fecha
                {
                    $comid = new Comida();
                    // Insertar datos en la tabla comidas
                    // La funcion date pasa la fecha de formato Linux a formato timestamp
                    $comid->fecha = date('Y:m:d H:i:s', $comida->fecha);
                    $comid->save();

                    foreach ($comida->platos as $plato)  // $comida->platos es un array de platos. $plato es un objeto cuya propiedad es comida_id
                    {
                        $plat = new Plato();
                        // Insertar datos en la tabla platos
                        $plat->comida_id = $comid->id;
                        $plat->save();

                        foreach ($plato->alimentos as $alimento)  // plato->alimentos es un array de alimentos. $alimento es un objeto cuyas propiedades son grupo y peso
                        {
                            $alim = new Alimento;
                            $alim->peso = $alimento->peso;
                            $alim->user_id = $user_id;
                            $alim->plato_id = $plat->id;

                            // ----- ALIMENTO DE NUESTROS GRUPOS -------------------------
                            // Si el alimento pertenece a un grupo definido (distinto de 50, que es el grupo barcode)
                            if ($alimento->grupo != 50) 
                            {
                                // Búsqueda del dato en la tabla grupos, cuyo campo cod_grupo coincida con el dato de grupo ($alimento->grupo) contenido en los parámetros de entrada del endpoint
                                $grup = Grupo::where('cod_grupo', $alimento->grupo)->first();
                                // Búsqueda del dato en la tabla tipo_alimentos, cuyo campo id coincida con el campo tipoalimento_id del dato $grup encontrado anteriormente
                                $tipo_alim = TipoAlimento::where('id', $grup->tipoalimento_id)->first();
                                $alim->tipoalimento_id = $tipo_alim->id;
                            }
                            // -----------------------------------------------------------
                            
                            // ----- ALIMENTO TIPO BARCODE -------------------------------
                            // Si el alimento es un producto con código de barras
                            else if (isset($alimento->ean)) 
                            {
                                $ean = $alimento->ean; // Obtener el código de barras del alimento

                                // Comprobar si el código de barras ya está en la base de datos
                                $barcode = Barcode::where('ean', $ean)->first();

                                // ----- BARCODE LEIDO ANTERIORMENTE ---------
                                if ($barcode) {
                                    $alim->tipoalimento_id = $barcode->tipoalimento_id; // Asignar el tipo de alimento del código de barras al alimento
                                }
                                // -------------------------------------------

                                // ----- NUNCA SE HA LEIDO ESTE BARCODE ------
                                else 
                                {
                                    // ----- BUSCAR INFO NUTRICIONAL -----
                                    // Obtener la información nutricional del producto desde OpenFoodFacts
                                    $productData = $this->getNutritionalDataFromOpenFoodFacts($ean);

                                    if ($productData) 
                                    {
                                        $nombre = $productData['nombre'];
                                        $carb_g = $productData['carb_g'] / 100.0; // Convertir los valores de por 100g a por gramo
                                        $kcal_g = $productData['kcal_g'] / 100.0;
                                        $lip_g = $productData['lip_g'] / 100.0;
                                        $prot_g = $productData['prot_g'] / 100.0;

                                        // ----- NUEVO TIPO ALIMENTO -----
                                        // Insertar el nuevo tipo de alimento en la base de datos
                                        $tipo_alim = new TipoAlimento;
                                        $tipo_alim->kcal_g = $kcal_g;
                                        $tipo_alim->prot_g = $prot_g;
                                        $tipo_alim->lip_g = $lip_g;
                                        $tipo_alim->carb_g = $carb_g;
                                        $tipo_alim->tipo = 'barcode';
                                        $tipo_alim->save();

                                        // ---- NUEVO PRODUCTO LEIDO -----
                                        // Insertar el código de barras en la base de datos
                                        $barcode = new Barcode;
                                        $barcode->ean = $ean;
                                        $barcode->nombre = $nombre;
                                        $barcode->tipoalimento_id = $tipo_alim->id;
                                        $barcode->save();

                                        // Asignar el tipo de alimento del código de barras al alimento
                                        $alim->tipoalimento_id = $tipo_alim->id;
                                    } 
                                    else {
                                        return response()->json([
                                            'success' => false,
                                            'message' => 'No se pudo obtener información del código de barras: ' . $ean
                                        ], 400);
                                    }
                                }
                                // -------------------------------------------
                            }
                            $alim->save();
                        }
                    }
                }
                return response()->json([
                    'success' => true,
                    'message' => 'Comidas para el mantel con MAC = ' . $datos->mac . ' insertadas correctamente!',
                    'comidas' => $datos
                ], 200);
            }
        }
    }

    /**
     * Obtiene los datos nutricionales de Open Food Facts para un código EAN dado.
     *
     * @param string $ean El código EAN del producto.
     * @return array|false Un array con los datos nutricionales del producto si se obtuvieron datos, o false si no se obtuvieron datos.
     */
    private function getNutritionalDataFromOpenFoodFacts($ean) {
        $url = "https://world.openfoodfacts.org/api/v2/product/" . $ean . "?fields=product_name,product_name_es,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g";
        
        $response = file_get_contents($url);    // Obtener la respuesta de la API de Open Food Facts
        $data = json_decode($response, true);   // Decodificar la respuesta JSON

        // Verificar si se obtuvieron datos del producto
        if (isset($data['product'])) {
            $product = $data['product'];   // Obtener los datos del producto

            // Crear un array con los datos nutricionales del producto
            return [
                'nombre' => isset($product['product_name_es']) ? $product['product_name_es'] : $product['product_name'],    // Tomar el nombre en español si está disponible
                'carb_g' => isset($product['carbohydrates_100g']) ? $product['carbohydrates_100g'] : 0.0,                   // Si no hay datos de carbohidratos, establecer a 0
                'kcal_g' => isset($product['energy-kcal_100g']) ? $product['energy-kcal_100g'] : 0.0,                       // Si no hay datos de calorías, establecer a 0
                'lip_g' => isset($product['fat_100g']) ? $product['fat_100g'] : 0.0,                                        // Si no hay datos de grasas, establecer a 0
                'prot_g'

 => isset($product['proteins_100g']) ? $product['proteins_100g'] : 0.0                              // Si no hay datos de proteínas, establecer a 0
            ];
        } else {
            return false;
        }
    }
}