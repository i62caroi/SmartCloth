/*


    -------- ESQUEMA ENTIDAD - RELACIÓN -----------------------------------------------
        ----- ENTIDADES Y ATRIBUTOS -------------
        - USUARIO: dni, nombre
        - MANTEL: id_mantel, activo
        - MEDICION: id_medicion, fecha_hora
        - INSULINA: valor
        - INDICE_GLUCEMICO: valor
        - COMIDA: id_comida, fecha, hora
        - PLATO: id_plato
        - ALIMENTO: id_alimento, peso
        - TIPO_ALIMENTO: id_tipo
        - GRUPO_ALIMENTO: id_grupo, nombre, ejemplos
        - BARCODE: EAN
        - VALORES_NUTRICIONALES: kcal_g, prot_g, lip_g, carb_g
        -----------------------------------------

        ------- RELACIONES --------------------
        - ALIMENTO es un par peso + TIPOALIMENTO
        - PLATO es la agregación de 1 o varios ALIMENTO 
        - COMIDA es la agregación de 1 o varios PLATO 
        - Cada ALIMENTO pertenecerá a un TIPOALIMENTO
        - Cada TIPOALIMENTO tendrá unos VALORESNUTRICIONALES
        - Cada TIPOALIMENTO podrá ser un GRUPOALIMENTO o BARCODE
        - Cada USUARIO tiene un MANTEL durante un periodo de tiempo y un MANTEL solo es de un USUARIO durante un periodo de tiempo.
        - Los USUARIO pueden tener MEDICION en fecha y hora, que podrá ser INSULINA o INDICEGLUCEMICO.
        -----------------------------------------

        ------- CARDINALIDADES ---------------
        - USUARIO (0,1) <-> (0,1) MANTEL
        - USUARIO (1,1) <-> (*,n) MEDICION
        - MEDICION (1,1) <-> (0,1) INSULINA
        - MEDICION (1,1) <-> (0,1) INDICE_GLUCEMICO
        - COMIDA (1,1) <-> (*,n) PLATO
        - PLATO (1,1) <-> (*,n) ALIMENTO
        - ALIMENTO (*,n) <-> (1,1) USUARIO
        - ALIMENTO (*,n) <-> (1,1) TIPO_ALIMENTO
        - TIPO_ALIMENTO (1,1) <-> (1,1) VALORES_NUTRICIONALES
        - TIPO_ALIMENTO (1,1) <-> (0,1) GRUPO_ALIMENTO
        - TIPO_ALIMENTO (1,1) <-> (0,1) BARCODE

            Nota: USUARIO se relaciona con (0,1) con MANTEL para el caso en que se le retire el mantel al usuario y MANTEL se relaciona
                  con (0,1) con USUARIO para el caso en que un mantel no esté asignado a ningún usuario porque esté averiado o no haya
                  usuarios libres.
        -----------------------------------------
    -----------------------------------------------------------------------------------


    -------- ESQUEMA RELACIONAL -------------------------------------------------------
    - USUARIO(dni, nombre) 

    - MANTEL(id_mantel, MAC, activo)

    - USUARIO-MANTEL(dni, id_mantel, fecha_inicio, fecha_fin) 
        USUARIO.dni 	 --> USUARIO-MANTEL.dni
	    MANTEL.id_mantel --> USUARIO-MANTEL.id_mantel

    - MEDICION (id_medicion, dni, fecha_hora)
        USUARIO.dni 	 --> MEDICION.dni

    - INSULINA (id_medicion, valor)
        MEDICION.id_medicion --> INSULINA.id_medicion
    
    - IND_GLUCEMICO (id_medicion, valor)
        MEDICION.id_medicion --> IND_GLUCEMICO.id_medicion
    
    - COMIDA(id_comida, fecha, hora) 
    
    - PLATO(id_plato, id_comida) 
        COMIDA.id_comida --> PLATO.id_comida
    
    - BARCODE(EAN, id_tipo) 
        TIPO_ALIMENTO.id_tipo --> BARCODE.id_tipo
    
    - GRUPO_ALIMENTO(grupo, id_tipo, nombre, ejemplos)
        TIPO_ALIMENTO.id_tipo --> GRUPO_ALIMENTO.id_tipo
    
    - TIPO_ALIMENTO(id_tipo, kcal_g, prot_g, lip_g, carb_g) 
    
    - ALIMENTO(id_alimento, peso, id_plato, id_tipo, dni) 
        PLATO.id_plato 		  --> ALIMENTO.id_plato
	    TIPO_ALIMENTO.id_tipo --> ALIMENTO.id_tipo
	    USUARIO.dni 		  --> ALIMENTO.dni

    En lugar de usar 'id_tipo' como PK en GrupoAlimento y Barcode, se usa 'grupo' y 'EAN',
    que son únicos y no nulos, para que el esquema sea más intuitivo y fácil de usar. 
    -----------------------------------------------------------------------------------


    RESUMEN DE LAS TABLAS:

    - `Usuario`: Almacena información sobre los usuarios, incluyendo su DNI (que es la clave primaria) y su nombre.
    - `Mantel`: Almacena información sobre los manteles, incluyendo un ID autoincrementado, una dirección MAC y un campo booleano para indicar si el mantel está activo.
    - `UsuarioMantel`: Almacena la relación entre los usuarios y los manteles, permitiendo el seguimiento de la propiedad del mantel a lo largo del tiempo.
    - `Medicion`: Almacena mediciones asociadas a cada usuario en fechas concretas.
    - `Insulina`: Almacena valores de insulina relacionados con mediciones concretas.
    - 'IndGlucemico': Almacena valores de índice glucémico relacionados con mediciones concretas.
    - `Comida`: Almacena información sobre las comidas, incluyendo la fecha y la hora.
    - `Plato`: Almacena información sobre los platos, que están asociados a una comida.
    - `TipoAlimento`: Almacena información sobre los tipos de alimentos, incluyendo su contenido nutricional, que pueden estar asociados a un grupo de alimentos o a un barcode.
    - `GrupoAlimento`: Almacena información sobre los diferentes grupos de alimentos, que están asociados a un tipo de alimento.
    - `Barcode`: Almacena los códigos de barras de los alimentos escaneados, que están asociados a un tipo de alimento
    - `Alimento`: Almacena información sobre los alimentos, que están asociados con un plato, un tipo de alimento y un usuario.
      
    Cada tabla tiene claves foráneas que hacen referencia a otras tablas, lo que permite la creación de relaciones entre las diferentes entidades de la base de datos.




*/


-- ---------- BORRAR TABLAS ANTES DE CREARLAS -------------------------
-- Primero se borran las tablas que no tienen FK que hagan referencia a ellas
-- y luego se borran las demás. Se suele empezar por las últimas creadas
DROP TABLE IF EXISTS Alimento;
DROP TABLE IF EXISTS Barcode;
DROP TABLE IF EXISTS GrupoAlimento;
DROP TABLE IF EXISTS TipoAlimento;
DROP TABLE IF EXISTS Plato;
DROP TABLE IF EXISTS Comida;
DROP TABLE IF EXISTS IndiceGlucemico;
DROP TABLE IF EXISTS Insulina;
DROP TABLE IF EXISTS Medicion;
DROP TABLE IF EXISTS UsuarioMantel;
DROP TABLE IF EXISTS Mantel;
DROP TABLE IF EXISTS Usuario;
-- --------------------------------------------------------------------



-- ---------- USUARIO -------------------------------------------------
CREATE TABLE Usuario (
    dni VARCHAR(9) PRIMARY KEY,
    nombre VARCHAR(100) NOT NULL
);
-- --------------------------------------------------------------------


-- ---------- MANTEL --------------------------------------------------
CREATE TABLE Mantel (
    id_mantel INT AUTO_INCREMENT PRIMARY KEY,
    MAC VARCHAR(17) NOT NULL,
    activo BOOLEAN NOT NULL DEFAULT FALSE
);
-- --------------------------------------------------------------------


-- ---------- USUARIO-MANTEL ------------------------------------------
/*  La tabla `UsuarioMantel` tiene tres campos en su clave primaria (`dni`, `id_mantel`, `fecha_inicio`) para 
    garantizar la unicidad de cada registro y permitir el seguimiento de la propiedad del mantel a lo largo del tiempo.
        - `dni`: Identifica al usuario. Necesitamos esto porque un usuario puede tener varios manteles a lo largo del tiempo.
        - `id_mantel`: Identifica el mantel. Necesitamos esto porque un mantel puede ser poseído por varios usuarios a lo largo 
            del tiempo.
        - `fecha_inicio`: Identifica cuándo comenzó la propiedad del mantel. Al incluirlo en la clave primaria, podemos tener 
            múltiples registros para el mismo usuario y mantel, siempre que los períodos de tiempo no se superpongan.
*/
CREATE TABLE UsuarioMantel (
    dni VARCHAR(9) NOT NULL,
    id_mantel INT NOT NULL,
    fecha_inicio DATE NOT NULL,
    fecha_fin DATE,
    PRIMARY KEY (dni, id_mantel, fecha_inicio),
    CONSTRAINT fk_Mantel_Usuario FOREIGN KEY (dni) REFERENCES Usuario(dni),
    CONSTRAINT fk_Usuario_Mantel FOREIGN KEY (id_mantel) REFERENCES Mantel(id_mantel)
);
-- --------------------------------------------------------------------


-- ---------- MEDICION ------------------------------------------------
CREATE TABLE Medicion (
    id_medicion INT AUTO_INCREMENT PRIMARY KEY,
    dni VARCHAR(9) NOT NULL,
    fecha_hora DATETIME NOT NULL,
    CONSTRAINT fk_Medicion_Usuario FOREIGN KEY (dni) REFERENCES Usuario(dni)
);
-- --------------------------------------------------------------------


-- ---------- INSULINA ------------------------------------------------
CREATE TABLE Insulina (
    id_medicion INT PRIMARY KEY,
    valor FLOAT NOT NULL, -- Valor de insulina
    CONSTRAINT fk_Insulina_Medicion FOREIGN KEY (id_medicion) REFERENCES Medicion(id_medicion)
);
-- --------------------------------------------------------------------


-- ---------- INDICE GLUCEMICO ------------------------------------------------
CREATE TABLE IndiceGlucemico (
    id_medicion INT PRIMARY KEY,
    valor FLOAT NOT NULL, -- Valor del índice glucémico
    CONSTRAINT fk_Glucemico_Medicion FOREIGN KEY (id_medicion) REFERENCES Medicion(id_medicion)
);
-- --------------------------------------------------------------------


-- ---------- COMIDA --------------------------------------------------
CREATE TABLE Comida (
    id_comida INT AUTO_INCREMENT PRIMARY KEY,
    -- 'fecha' y 'hora' como VARCHAR porque así se envían desde el RTC
    -- Se podrían transformar a DATETIME en el servidor
    fecha VARCHAR(12) NOT NULL, 
    hora VARCHAR(12) NOT NULL
);
-- --------------------------------------------------------------------


-- ---------- PLATO ---------------------------------------------------
CREATE TABLE Plato (
    id_plato INT AUTO_INCREMENT PRIMARY KEY,
    id_comida INT NOT NULL, -- comida de la que forma parte el plato
    CONSTRAINT fk_Plato_Comida FOREIGN KEY (id_comida) REFERENCES Comida(id_comida)
);
-- --------------------------------------------------------------------


-- ---------- TIPO ALIMENTO -------------------------------------------
CREATE TABLE TipoAlimento (
    id_tipo INT AUTO_INCREMENT PRIMARY KEY,
    kcal_g FLOAT,
    prot_g FLOAT,
    lip_g FLOAT,
    carb_g FLOAT
);
-- --------------------------------------------------------------------


-- ---------- GRUPO ALIMENTO ------------------------------------------
CREATE TABLE GrupoAlimento (
    grupo INT PRIMARY KEY,  -- código del grupo
    id_tipo INT NOT NULL,   -- tipo de alimento al que hace referencia
    nombre VARCHAR(100),    -- nombre del grupo de alimentos
    ejemplos TEXT,          -- ejemplos del grupo de alimentos
    CONSTRAINT fk_Grupo_Tipo FOREIGN KEY (id_tipo) REFERENCES TipoAlimento(id_tipo)
);
-- --------------------------------------------------------------------


-- ---------- BARCODE -------------------------------------------------
CREATE TABLE Barcode (
    EAN VARCHAR(13) PRIMARY KEY, 
    id_tipo INT NOT NULL, -- tipo de alimento al que hace referencia
    CONSTRAINT fk_Barcode_Tipo FOREIGN KEY (id_tipo) REFERENCES TipoAlimento(id_tipo)
);
-- --------------------------------------------------------------------


-- ---------- ALIMENTO ------------------------------------------------
CREATE TABLE Alimento (
    id_alimento INT AUTO_INCREMENT PRIMARY KEY,
    peso FLOAT NOT NULL,
    id_plato INT NOT NULL, -- plato del que forma parte el alimento
    id_tipo INT NOT NULL,  -- tipo del que es el alimento (puede ser grupo o barcode)
    dni VARCHAR(9),        -- usuario que ha consumido el alimento 
    CONSTRAINT fk_Alimento_Plato FOREIGN KEY (id_plato) REFERENCES Plato(id_plato),
    CONSTRAINT fk_Alimento_Tipo FOREIGN KEY (id_tipo) REFERENCES TipoAlimento(id_tipo),
    CONSTRAINT fk_Alimento_Usuario FOREIGN KEY (dni) REFERENCES Usuario(dni)
);
-- --------------------------------------------------------------------
