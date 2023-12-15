/*

    -------- ESQUEMA ENTIDAD - RELACIÓN -----------------------------------------------
        ----- ENTIDADES Y ATRIBUTOS -------------
        - USUARIO: dni, nombre
        - MANTEL: id_mantel, activo
        - MEDICION: id_medicion, fecha_hora
        - INSULINA: insulina
        - INDICE_GLUCEMICO: indice_glucemico
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
    - MANTEL(id_mantel, activo)
    - USUARIO-MANTEL(dni, id_mantel, fecha_inicio, fecha_fin) 
        USUARIO.dni 	 --> USUARIO-MANTEL.dni
	    MANTEL.id_mantel --> USUARIO-MANTEL.id_mantel
    - MEDICION (id_medicion, dni, fecha_hora, insulina, indice_glucemico) 
    - COMIDA(id_comida, fecha, hora) 
    - PLATO(id_plato, id_comida) 
        COMIDA.id_comida --> PLATO.id_comida
    - BARCODE(EAN) 
    - GRUPO_ALIMENTO_BASE(id_base, nombre_grupo, ejemplos_grupo)
    - GRUPO_ALIMENTO(id_grupo, id_base)
        GRUPO_ALIMENTO_BASE.id_base --> GRUPO_ALIMENTO.id_base
    - TIPO_ALIMENTO(id_tipo, id_grupo, EAN) 
        GRUPO_ALIMENTO.id_grupo --> TIPO_ALIMENTO.id_grupo
	    BARCODE.EAN 			--> TIPO_ALIMENTO.EAN
    - ALIMENTO(id_alimento, peso, id_plato, id_tipo, dni) 
        PLATO.id_plato 		  --> ALIMENTO.id_plato
	    TIPO_ALIMENTO.id_tipo --> ALIMENTO.id_tipo
	    USUARIO.dni 		  --> ALIMENTO.dni
    - VALORES_NUTRICIONALES(id_tipo, kcal_g, prot_g, lip_g, carb_g) 
	    TIPO_ALIMENTO.id_tipo --> VALORES_NUTRICIONALES.id_tipo
    -----------------------------------------------------------------------------------


    RESUMEN DE LAS TABLAS:

    - `Usuario`: Almacena información sobre los usuarios.
    - `Mantel`: Almacena información sobre los manteles.
    - `UsuarioMantel`: Almacena la relación entre los usuarios y los manteles, permitiendo el seguimiento de la propiedad del mantel a lo largo del tiempo.
    - `Medicion`: Almacena las mediciones de insulina e índice glucémico de los usuarios.
    - `Comida`: Almacena información sobre las comidas.
    - `Plato`: Almacena la relación entre los platos y las comidas.
    - 'GrupoAlimentoBase': almacena información base general (nombre y ejemplos) de los grupos de alimentos.
    - `GrupoAlimento`: Almacena información (id y enlace a info base) sobre los diferentes grupos de alimentos.
    - `Barcode`: Almacena los códigos de barras de los alimentos escaneados.
    - `TipoAlimento`: Almacena información sobre los diferentes tipos de alimentos, que pueden ser identificados por un grupo de alimentos o un código de barras.
    - `Alimento`: Almacena información sobre los alimentos, que están asociados con un plato, un tipo de alimento y un usuario.
    - `ValoresNutricionales`: Almacena los valores nutricionales de los diferentes tipos de alimentos.

    ¿¿NO SERÍA MÁS LÓGICO RELACIONAR EL USUARIO CON LA COMIDA EN LUGAR DEL ALIMENTO??
        |
        --> NO
            |
            --> Ya entiendo por qué se relaciona USUARIO con ALIMENTO en lugar de COMIDA: mi idea era que se podría relacionar USUARIO directamente con COMIDA 
                teniendo en cuenta que una comida consta de uno o varios platos y que cada plato incluye uno o varios alimentos. Sin embargo, a nivel relacional 
                es en la tabla ALIMENTO donde se tiene una referencia (FK) al plato en el que se encuentra ese alimento y en la tabla PLATO se tiene una referencia 
                a la comida de la que forma parte. 
                Por eso se relaciona USUARIO con ALIMENTO y no con COMIDA, porque si se relacionara con COMIDA, se podría saber cuándo ha comido un usuario, 
                pero no qué alimentos ha consumido, ya que COMIDA no tiene referencias a las otras entidades.
                
*/


-- ---------- BORRAR TABLAS ANTES DE CREARLAS -------------------------
-- Primero se borran las tablas que no tienen FK que hagan referencia a ellas
-- y luego se borran las demás. Se suele empezar por las últimas creadas
DROP TABLE IF EXISTS ValoresNutricionales;
DROP TABLE IF EXISTS Alimento;
DROP TABLE IF EXISTS TipoAlimento;
DROP TABLE IF EXISTS Barcode;
DROP TABLE IF EXISTS GrupoAlimento;
DROP TABLE IF EXISTS GrupoAlimentoBase;
DROP TABLE IF EXISTS Plato;
DROP TABLE IF EXISTS Comida;
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
    id_mantel INT AUTO_INCREMENT PRIMARY KEY
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
    FOREIGN KEY (dni) REFERENCES Usuario(dni),
    FOREIGN KEY (id_mantel) REFERENCES Mantel(id_mantel)
);
-- --------------------------------------------------------------------


-- ---------- MEDICION ------------------------------------------------
CREATE TABLE Medicion (
    id_medicion INT AUTO_INCREMENT PRIMARY KEY,
    dni VARCHAR(9) NOT NULL,
    fecha_hora DATETIME NOT NULL,
    insulina FLOAT,
    indice_glucemico FLOAT,
    FOREIGN KEY (dni) REFERENCES Usuario(dni),
    -- Comprobar exclusividad: una medición puede ser insulina o índice glucémico, pero no ambos
    -- Si el usuario quiere guardar ambos valores, debe hacer dos mediciones
    CHECK ((insulina IS NOT NULL AND indice_glucemico IS NULL) OR (insulina IS NULL AND indice_glucemico IS NOT NULL))
    -- Si se van a guardar los dos valores a la vez, se debe quitar esta restricción CHECK
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
    FOREIGN KEY (id_comida) REFERENCES Comida(id_comida)
);
-- --------------------------------------------------------------------


-- ---------- BARCODE -------------------------------------------------
CREATE TABLE Barcode (
    EAN VARCHAR(13) PRIMARY KEY
);
-- --------------------------------------------------------------------


-- ---------- GRUPO ALIMENTO BASE -------------------------------------
/*  La tabla 'GrupoAlimentoBase' guarda los nombres y ejemplos de los 20 grupos oficiales. Luego a cada
    'GrupoAlimento' se le asigna su 'id_base' con esa información. 
    Se ha incluido esta tabla separada para no duplicar nombres y ejemplos en los casos de grupos con 
    diferentes valores nutricionales según si el alimento es crudo o cocinado, ya que se consideran
    como dos grupos separados pero tienen la misma información base (nombre y ejemplos).
*/
CREATE TABLE GrupoAlimentoBase (
    id_base INT PRIMARY KEY,
    nombre_grupo VARCHAR(100) NOT NULL,
    ejemplos_grupo TEXT
);
-- --------------------------------------------------------------------


-- ---------- GRUPO ALIMENTO ------------------------------------------
CREATE TABLE GrupoAlimento (
    id_grupo INT PRIMARY KEY,
    id_base INT NOT NULL, -- información de 'nombre' y 'ejemplos'
    FOREIGN KEY (id_base) REFERENCES GrupoAlimentoBase(id_base)
);
-- --------------------------------------------------------------------


-- ---------- TIPO ALIMENTO -------------------------------------------
CREATE TABLE TipoAlimento (
    id_tipo INT AUTO_INCREMENT PRIMARY KEY,
    -- Se usa UNIQUE para garantizar que no se creen tipos de alimentos  
    -- duplicados para el mismo grupo o barcode
    id_grupo INT UNIQUE, 
    EAN VARCHAR(13) UNIQUE,
    FOREIGN KEY (id_grupo) REFERENCES GrupoAlimento(id_grupo),
    FOREIGN KEY (EAN) REFERENCES Barcode(EAN),
    -- Comprobar exclusividad: un tipo de alimento puede ser grupo o barcode, pero no ambos
    CHECK ((id_grupo IS NULL AND EAN IS NOT NULL) OR (id_grupo IS NOT NULL AND EAN IS NULL))
);
-- --------------------------------------------------------------------


-- ---------- ALIMENTO ------------------------------------------------
CREATE TABLE Alimento (
    id_alimento INT AUTO_INCREMENT PRIMARY KEY,
    peso FLOAT NOT NULL,
    id_plato INT NOT NULL, -- plato del que forma parte el alimento
    id_tipo INT NOT NULL,  -- tipo del que es el alimento (puede ser grupo o barcode)
    dni VARCHAR(9),        -- usuario que ha consumido el alimento 
    FOREIGN KEY (id_plato) REFERENCES Plato(id_plato),
    FOREIGN KEY (id_tipo) REFERENCES TipoAlimento(id_tipo),
    FOREIGN KEY (dni) REFERENCES Usuario(dni)
);
-- --------------------------------------------------------------------


-- ---------- VALORES NUTRICIONALES -----------------------------------
CREATE TABLE ValoresNutricionales (
    id_tipo INT PRIMARY KEY, -- tipo de alimento (grupo o barcode) que tiene estos macronutrientes por gramo
    kcal_g FLOAT,
    prot_g FLOAT,
    lip_g FLOAT,
    carb_g FLOAT,
    FOREIGN KEY (id_tipo) REFERENCES TipoAlimento(id_tipo)
);
-- --------------------------------------------------------------------

