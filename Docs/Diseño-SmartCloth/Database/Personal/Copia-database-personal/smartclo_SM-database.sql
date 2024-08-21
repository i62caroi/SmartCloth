-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Servidor: 192.168.0.100
-- Tiempo de generación: 09-05-2024 a las 14:24:08
-- Versión del servidor: 8.0.36-28
-- Versión de PHP: 7.3.13

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de datos: `smartclo_SM-database`
--

DELIMITER $$
--
-- Procedimientos
--
CREATE DEFINER=`smartclo`@`%` PROCEDURE `InsertarGrupoYTipo` (IN `grupo` INT, IN `kcal_g` FLOAT, IN `prot_g` FLOAT, IN `lip_g` FLOAT, IN `carb_g` FLOAT, IN `nombre_grupo` VARCHAR(255), IN `ejemplos_grupo` TEXT)   BEGIN
    INSERT INTO TipoAlimento (kcal_g, prot_g, lip_g, carb_g) VALUES (kcal_g, prot_g, lip_g, carb_g);
    SET @id_tipo = LAST_INSERT_ID();
    INSERT INTO GrupoAlimento (grupo, id_tipo, nombre, ejemplos) VALUES (grupo, @id_tipo, nombre_grupo, ejemplos_grupo);
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Alimento`
--

CREATE TABLE `Alimento` (
  `id_alimento` int NOT NULL,
  `peso` float NOT NULL,
  `id_plato` int NOT NULL,
  `id_tipo` int NOT NULL,
  `dni` varchar(9) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `Alimento`
--

INSERT INTO `Alimento` (`id_alimento`, `peso`, `id_plato`, `id_tipo`, `dni`) VALUES
(1, 53.5, 1, 15, '30987645F'),
(2, 53.5, 1, 9, '30987645F'),
(3, 24.4, 2, 15, '30987645F'),
(4, 53.5, 3, 15, '30987645F'),
(5, 53.5, 3, 9, '30987645F'),
(6, 32.07, 4, 3, '30987645F'),
(7, 46.65, 4, 21, '30987645F'),
(8, 16.23, 4, 10, '30987645F'),
(9, 53.5, 5, 15, '38042651J'),
(10, 53.5, 5, 9, '38042651J'),
(11, 24.4, 6, 15, '38042651J'),
(12, 53.5, 7, 15, '38042651J'),
(13, 53.5, 7, 9, '38042651J'),
(14, 32.07, 8, 3, '38042651J'),
(15, 53.5, 9, 15, '38042651J'),
(16, 53.5, 9, 9, '38042651J'),
(17, 24.4, 10, 15, '38042651J'),
(18, 53.5, 11, 15, '38042651J'),
(19, 53.5, 11, 9, '38042651J'),
(20, 24.4, 12, 15, '38042651J'),
(21, 53.5, 13, 15, '38042651J'),
(22, 53.5, 13, 9, '38042651J'),
(23, 32.07, 14, 3, '38042651J'),
(24, 46.65, 14, 21, '38042651J'),
(25, 16.23, 14, 10, '38042651J'),
(26, 53.5, 15, 15, '38042651J'),
(27, 53.5, 15, 9, '38042651J'),
(28, 24.4, 16, 15, '38042651J'),
(29, 53.5, 17, 15, '38042651J'),
(30, 53.5, 17, 9, '38042651J'),
(31, 24.4, 18, 15, '38042651J'),
(32, 53.5, 19, 15, '38042651J'),
(33, 53.5, 19, 9, '38042651J'),
(34, 24.4, 20, 15, '38042651J'),
(35, 24.4, 21, 15, '38042651J'),
(36, 53.5, 22, 15, '38042651J'),
(37, 53.5, 22, 9, '38042651J'),
(38, 24.4, 23, 15, '38042651J'),
(39, 53.5, 24, 15, '38042651J'),
(40, 53.5, 24, 9, '38042651J'),
(41, 24.4, 25, 15, '38042651J'),
(42, 53.5, 26, 15, '38042651J'),
(43, 53.5, 26, 9, '38042651J'),
(44, 24.4, 27, 15, '38042651J'),
(45, 53.5, 28, 15, '38042651J'),
(46, 53.5, 28, 9, '38042651J'),
(47, 24.4, 29, 15, '38042651J'),
(48, 53.5, 30, 15, '38042651J'),
(49, 53.5, 30, 9, '38042651J'),
(50, 24.4, 31, 15, '38042651J'),
(51, 53.5, 32, 15, '38042651J'),
(52, 53.5, 32, 9, '38042651J'),
(53, 24.4, 33, 15, '38042651J'),
(54, 53.5, 34, 15, '38042651J'),
(55, 53.5, 34, 9, '38042651J'),
(56, 24.4, 35, 15, '38042651J'),
(57, 53.5, 36, 15, '38042651J'),
(58, 53.5, 36, 9, '38042651J'),
(59, 24.4, 37, 15, '38042651J'),
(60, 53.5, 38, 15, '38042651J'),
(61, 53.5, 38, 9, '38042651J'),
(62, 24.4, 39, 15, '38042651J'),
(63, 53.5, 40, 15, '38042651J'),
(64, 53.5, 40, 9, '38042651J'),
(65, 24.4, 41, 15, '38042651J'),
(66, 53.5, 42, 15, '38042651J'),
(67, 53.5, 42, 9, '38042651J'),
(68, 24.4, 43, 15, '38042651J'),
(69, 53.5, 44, 15, '38042651J'),
(70, 53.5, 44, 9, '38042651J'),
(71, 32.07, 45, 3, '38042651J'),
(72, 46.65, 45, 21, '38042651J'),
(73, 16.23, 45, 10, '38042651J'),
(74, 53.5, 46, 15, '38042651J'),
(75, 53.5, 46, 9, '38042651J'),
(76, 24.4, 47, 15, '38042651J'),
(77, 53.5, 48, 15, '38042651J'),
(78, 53.5, 48, 9, '38042651J'),
(79, 24.4, 49, 15, '38042651J');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `alimento`
--

CREATE TABLE `alimento` (
  `ID` int NOT NULL,
  `grupo` int NOT NULL,
  `peso` float NOT NULL,
  `plato_id` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `alimento`
--

INSERT INTO `alimento` (`ID`, `grupo`, `peso`, `plato_id`) VALUES
(1, 15, 53.5, 1),
(2, 9, 53.5, 1),
(3, 15, 24.4, 2),
(4, 15, 53.5, 3),
(5, 9, 53.5, 3),
(6, 3, 32.07, 4),
(7, 27, 46.65, 4),
(8, 10, 16.23, 4);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Barcode`
--

CREATE TABLE `Barcode` (
  `EAN` varchar(13) NOT NULL,
  `id_tipo` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Comida`
--

CREATE TABLE `Comida` (
  `id_comida` int NOT NULL,
  `fecha` timestamp NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `Comida`
--

INSERT INTO `Comida` (`id_comida`, `fecha`) VALUES
(1, '2005-12-20 21:00:00'),
(2, '2005-12-20 21:00:00'),
(3, '2020-12-20 20:00:00'),
(4, '2005-12-20 21:00:00'),
(5, '2020-12-20 20:00:00'),
(6, '2020-12-20 20:00:00'),
(7, '2005-12-20 21:00:00'),
(8, '2005-12-20 21:00:00'),
(9, '0000-00-00 00:00:00'),
(10, '0000-00-00 00:00:00'),
(11, '0000-00-00 00:00:00'),
(12, '2005-12-20 21:00:00'),
(13, '2005-12-20 21:00:00'),
(14, '2005-12-20 21:00:00'),
(15, '2005-12-20 21:00:00'),
(16, '2005-12-20 21:00:00'),
(17, '2005-12-20 00:03:00'),
(18, '2005-12-20 21:00:00'),
(19, '2005-12-20 21:00:00'),
(20, '2005-12-20 21:00:00'),
(21, '2005-12-20 21:00:00'),
(22, '2005-12-20 03:00:00'),
(23, '2005-12-20 21:00:00'),
(24, '2005-12-20 21:00:00'),
(25, '2005-12-20 21:00:00'),
(26, '2005-12-20 21:00:00'),
(27, '2005-12-20 21:00:00'),
(28, '2005-12-20 21:00:00'),
(29, '2024-01-19 17:55:36'),
(30, '2024-01-19 18:25:25'),
(31, '2024-01-19 15:55:36'),
(32, '2024-01-19 16:25:25'),
(33, '2024-01-19 14:55:36'),
(34, '2024-01-19 15:25:25'),
(35, '2024-01-19 14:55:36'),
(36, '2024-01-19 15:25:25'),
(37, '2024-01-19 14:55:36'),
(38, '2024-01-19 15:25:25'),
(39, '2024-01-19 14:55:36'),
(40, '2024-01-19 15:25:25'),
(41, '2024-01-19 07:39:36'),
(42, '2023-12-05 14:55:36'),
(43, '2023-12-05 15:25:25'),
(44, '2023-12-05 14:55:36'),
(45, '2023-12-05 15:25:25');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `comida`
--

CREATE TABLE `comida` (
  `ID` int NOT NULL,
  `fecha` varchar(12) NOT NULL,
  `hora` varchar(12) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `comida`
--

INSERT INTO `comida` (`ID`, `fecha`, `hora`) VALUES
(1, '05.12.2023', '17:55:36'),
(2, '05.12.2023', '18:25:25'),
(3, '12.12.2023', '13:39:36');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `comidas`
--

CREATE TABLE `comidas` (
  `ID` int NOT NULL,
  `fechaHora` timestamp NOT NULL,
  `carb` float NOT NULL,
  `carb_R` float NOT NULL,
  `lip` float NOT NULL,
  `lip_R` float NOT NULL,
  `prot` float NOT NULL,
  `prot_R` float NOT NULL,
  `kcal` float NOT NULL,
  `peso` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `comidas`
--

INSERT INTO `comidas` (`ID`, `fechaHora`, `carb`, `carb_R`, `lip`, `lip_R`, `prot`, `prot_R`, `kcal`, `peso`) VALUES
(1, '2023-10-16 16:09:13', 17.2, 2, 32.1, 3, 45.7, 4.5, 176.12, 256.89),
(2, '2023-10-17 08:34:17', 13, 1, 10, 1, 9, 1, 178, 105),
(3, '2023-10-17 08:34:48', 8, 1, 21, 2, 14, 1, 277, 59),
(4, '2023-10-17 08:35:19', 25, 3, 18, 2, 5, 1, 282, 63),
(5, '2023-10-17 08:35:49', 13, 1, 11, 1, 10, 1, 191, 64),
(6, '2023-10-17 08:36:20', 9, 1, 28, 3, 13, 1, 340, 106),
(7, '2023-10-17 08:36:50', 13, 1, 29, 3, 21, 2, 397, 137),
(8, '2023-10-17 08:37:21', 22, 2, 11, 1, 8, 1, 219, 77),
(9, '2023-10-17 08:37:52', 14, 1, 25, 3, 13, 1, 333, 131),
(10, '2023-10-17 08:38:23', 8, 1, 27, 3, 25, 3, 375, 129),
(11, '2023-10-17 08:38:53', 19, 2, 5, 1, 29, 3, 237, 83),
(12, '2023-10-17 09:25:51', 18.9, 2, 37.8, 4, 43.1, 4, 78.34, 134.57),
(13, '2023-10-17 09:47:58', 20, 2, 18, 2, 18, 2, 314, 80),
(14, '2023-10-17 09:48:59', 13, 1, 28, 3, 15, 2, 364, 55),
(15, '2023-10-17 09:50:00', 23, 2, 21, 2, 23, 2, 373, 100),
(16, '2023-10-17 09:51:00', 20, 2, 24, 2, 17, 2, 364, 99),
(17, '2023-10-17 09:56:03', 11, 1, 28, 3, 12, 1, 344, 133),
(18, '2023-10-17 09:56:39', 18, 2, 9, 1, 5, 1, 173, 97),
(19, '2023-10-17 09:57:53', 26, 3, 9, 1, 11, 1, 229, 72),
(20, '2023-10-17 10:03:19', 10, 1, 15, 2, 5, 1, 195, 79),
(21, '2023-10-17 10:08:32', 25, 3, 22, 2, 18, 2, 370, 143),
(22, '2023-10-17 10:13:04', 27, 3, 8, 1, 8, 1, 212, 132),
(23, '2023-10-17 10:41:00', 10, 1, 16, 2, 19, 2, 260, 85),
(24, '2023-10-17 10:41:17', 27, 3, 8, 1, 21, 2, 264, 101),
(25, '2023-10-17 10:46:18', 17, 2, 23, 2, 17, 2, 343, 112),
(26, '2023-10-17 10:51:19', 20, 2, 19, 2, 27, 3, 359, 91),
(27, '2023-10-17 10:56:20', 13, 1, 24, 2, 17, 2, 336, 133),
(28, '2023-10-17 11:00:07', 20, 2, 19, 2, 28, 3, 363, 102),
(29, '2023-10-17 11:04:05', 29.5, 3, 23.5, 2, 27.5, 3, 439.5, 146.5),
(30, '2023-10-17 11:09:05', 11.5, 1, 18.5, 2, 8.5, 1, 246.5, 121.5),
(31, '2023-10-17 11:14:06', 7.5, 1, 7.5, 1, 21.5, 2, 183.5, 88.5),
(32, '2023-10-17 11:19:06', 19.5, 2, 29.5, 3, 8.5, 1, 377.5, 80.5),
(33, '2023-10-17 11:24:07', 12.5, 1, 20.5, 2, 25.5, 3, 336.5, 145.5),
(34, '2023-10-17 11:29:08', 17.5, 2, 18.5, 2, 21.5, 2, 322.5, 72.5),
(35, '2023-10-17 11:34:09', 22.5, 2, 13.5, 1, 27.5, 3, 321.5, 93.5),
(36, '2023-10-17 11:39:09', 17.5, 2, 10.5, 1, 13.5, 1, 218.5, 142.5),
(37, '2023-10-17 11:44:10', 6.5, 1, 10.5, 1, 27.5, 3, 230.5, 83.5),
(38, '2023-10-17 11:49:11', 28.5, 3, 9.5, 1, 17.5, 2, 269.5, 138.5),
(39, '2023-10-17 11:54:12', 15.5, 2, 9.5, 1, 19.5, 2, 225.5, 146.5),
(40, '2023-10-31 12:26:40', 11.5, 1, 20.5, 2, 9.5, 1, 268.5, 123.5),
(41, '2023-10-31 10:56:22', 24.5, 2, 9.5, 1, 12.5, 1, 233.5, 134.5),
(42, '2023-10-31 11:01:14', 5.5, 1, 21.5, 2, 6.5, 1, 241.5, 136.5),
(43, '2023-10-31 13:56:52', 7.5, 1, 21.5, 2, 22.5, 2, 313.5, 94.5),
(44, '2023-10-31 14:04:14', 22.5, 2, 9.5, 1, 7.5, 1, 205.5, 113.5),
(45, '2023-11-03 13:52:19', 13.5, 1, 23.5, 2, 17.5, 2, 335.5, 79.5),
(46, '2023-11-03 14:29:59', 13.5, 1, 23.5, 2, 17.5, 2, 335.5, 79.5),
(47, '2023-11-03 14:31:44', 13.5, 1, 23.5, 2, 17.5, 2, 335.5, 79.5),
(48, '2023-11-03 14:33:11', 13.5, 1, 23.5, 2, 17.5, 2, 335.5, 79.5),
(49, '2023-11-03 14:37:45', 13.5, 1, 23.5, 2, 17.5, 2, 335.5, 79.5),
(50, '2023-12-12 09:36:28', 28.6, 3, 14.5, 1.5, 45.6, 4.5, 254.78, 154.65);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `failed_jobs`
--

CREATE TABLE `failed_jobs` (
  `id` bigint UNSIGNED NOT NULL,
  `uuid` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `connection` text COLLATE utf8mb4_unicode_ci NOT NULL,
  `queue` text COLLATE utf8mb4_unicode_ci NOT NULL,
  `payload` longtext COLLATE utf8mb4_unicode_ci NOT NULL,
  `exception` longtext COLLATE utf8mb4_unicode_ci NOT NULL,
  `failed_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `GrupoAlimento`
--

CREATE TABLE `GrupoAlimento` (
  `grupo` int NOT NULL,
  `id_tipo` int NOT NULL,
  `nombre` varchar(100) DEFAULT NULL,
  `ejemplos` text
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `GrupoAlimento`
--

INSERT INTO `GrupoAlimento` (`grupo`, `id_tipo`, `nombre`, `ejemplos`) VALUES
(1, 1, 'Lácteos enteros', 'Leche entera de vaca (pasteurizada o UHT), de oveja, de cabra, yogurt natural entero, cuajada, etc.'),
(2, 2, 'Lácteos desnatados', 'Leche desnatada, yogurt natural desnatado, etc.'),
(3, 3, 'Lácteos desnatados', 'Leche desnatada pasteurizada y UHT, natural, con frutas, yogurt desnatado, yogurt desnatado de sabores, etc.'),
(4, 4, 'Lácteos azucarados', 'Batidos lácteos de cacao y otros sabores, leche entera fermentada con frutas, yogures enteros de sabores y azucarados, yogures líquidos de sabores y azucarados'),
(5, 5, 'Postres lácteos', 'Arroz con leche, flan de huevo, flan de vainilla y natillas...'),
(6, 6, 'Frutas frescas, desecadas y zumos', 'Albaricoque, arándanos, cerezas, ciruelas, dátil seco, fresa, granada, higos, kiwi, mandarina,manzana, melocotón, melón, naranja, pera, piña, plátano, sandía, uvas...'),
(7, 7, 'Verduras y hortalizas', 'Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza, champiñones, col, espárragos, espinacas, guisantes, lechuga, judías, pimientos, tomate, zanahoria, etc.'),
(8, 8, 'Cereales y tubérculos', 'Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra, copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.'),
(9, 9, 'Legumbres', 'Alubias, garbanzos, lentejas, etc.'),
(10, 10, 'Repostería, pastelería y otros', 'Bizcocho, bollo, croissant, ensaimada, galletas (de cualquier tipo), magdalena, muesli, pan de pasas, tartas, pasteles, bollería industrial...'),
(11, 11, 'Alimentos ricos en grasas saludables', 'Aceites de cacahuete, de oliva y de hígado de bacalao, aceitunas, aguacate, almendras, avellanas, cacahuetes, pistachos, mayonesa de aceite de oliva, etc.'),
(12, 12, 'Alimentos ricos en grasas vegetales', 'Aceite de girasol, aceite de maíz, aceite de soja, mayonesa light, nueces, piñones, etc.'),
(13, 13, 'Alimentos ricos en grasas saturadas', 'Coco fresco o seco, aceite de coco, mantequilla, nata líquida para cocinar o montar, etc.'),
(14, 14, 'Alimentos muy grasos (mezclas)', 'Margarina light, margarina vegetal enriquecida, manteca y tocino de cerdo'),
(15, 15, 'Azúcares y dulces', 'Azúcar blanco y moreno, miel, leche condensada, cacao soluble azucarado, confitura de fruta baja en calorías'),
(16, 16, 'Alimentos proteicos con muy poca grasa', 'Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural, pescado no graso, marisco, queso granulado, clara de huevo, etc.'),
(17, 17, 'Alimentos proteicos con poca grasa', 'Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato sin piel, codorniz, etc.'),
(18, 18, 'Alimentos proteicos semigrasos', 'Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa, salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en porciones, etc.'),
(19, 19, 'Alimentos proteicos grasos', 'Chuletas/costillas de cordero, chorizo, salchichas, fuet, quesos (azul, babybel, camembert, cheddar, de cabra, emmental, gouda, gruyer, manchego...)'),
(20, 20, 'Alimentos proteicos muy grasos', 'Carne picada sazonada, panceta de cerdo, morcilla, mortadela, paté, salami, salchichón, etc.'),
(27, 21, 'Verduras y hortalizas', 'Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza, champiñones, col, espárragos, espinacas, guisantes, lechuga, judías, pimientos, tomate, zanahoria, etc.'),
(28, 22, 'Cereales y tubérculos', 'Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra, copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.'),
(29, 23, 'Legumbres', 'Alubias, garbanzos, lentejas, etc.'),
(36, 24, 'Alimentos proteicos con muy poca grasa', 'Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural, pescado no graso, marisco, queso granulado, clara de huevo, etc.'),
(37, 25, 'Alimentos proteicos con poca grasa', 'Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato sin piel, codorniz, etc.'),
(38, 26, 'Alimentos proteicos semigrasos', 'Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa, salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en porciones, etc.');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `IndiceGlucemico`
--

CREATE TABLE `IndiceGlucemico` (
  `id_medicion` int NOT NULL,
  `valor` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Insulina`
--

CREATE TABLE `Insulina` (
  `id_medicion` int NOT NULL,
  `valor` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Mantel`
--

CREATE TABLE `Mantel` (
  `id_mantel` int NOT NULL,
  `MAC` varchar(17) NOT NULL,
  `activo` tinyint(1) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `Mantel`
--

INSERT INTO `Mantel` (`id_mantel`, `MAC`, `activo`) VALUES
(1, '08:D1:F9:CB:A1:EC', 1),
(2, '08:D1:F9:CB:4C:24', 1),
(3, '08:D1:F9:DE:F0:12', 0);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Medicion`
--

CREATE TABLE `Medicion` (
  `id_medicion` int NOT NULL,
  `dni` varchar(9) NOT NULL,
  `fecha_hora` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `migrations`
--

CREATE TABLE `migrations` (
  `id` int UNSIGNED NOT NULL,
  `migration` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `batch` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Volcado de datos para la tabla `migrations`
--

INSERT INTO `migrations` (`id`, `migration`, `batch`) VALUES
(1, '2014_10_12_000000_create_users_table', 1),
(2, '2014_10_12_100000_create_password_resets_table', 1),
(3, '2019_08_19_000000_create_failed_jobs_table', 1),
(4, '2019_12_14_000001_create_personal_access_tokens_table', 1),
(5, '2023_10_28_132732_create_products_table', 1);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `password_resets`
--

CREATE TABLE `password_resets` (
  `email` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `token` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `created_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `personal_access_tokens`
--

CREATE TABLE `personal_access_tokens` (
  `id` bigint UNSIGNED NOT NULL,
  `tokenable_type` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `tokenable_id` bigint UNSIGNED NOT NULL,
  `name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `token` varchar(64) COLLATE utf8mb4_unicode_ci NOT NULL,
  `abilities` text COLLATE utf8mb4_unicode_ci,
  `last_used_at` timestamp NULL DEFAULT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `plato`
--

CREATE TABLE `plato` (
  `ID` int NOT NULL,
  `comida_id` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `plato`
--

INSERT INTO `plato` (`ID`, `comida_id`) VALUES
(1, 1),
(2, 2),
(3, 3),
(4, 3);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Plato`
--

CREATE TABLE `Plato` (
  `id_plato` int NOT NULL,
  `id_comida` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `Plato`
--

INSERT INTO `Plato` (`id_plato`, `id_comida`) VALUES
(1, 1),
(2, 2),
(3, 3),
(4, 3),
(5, 4),
(6, 5),
(7, 6),
(8, 6),
(9, 7),
(10, 8),
(11, 9),
(12, 10),
(13, 11),
(14, 11),
(15, 12),
(16, 13),
(17, 14),
(18, 15),
(19, 16),
(20, 17),
(21, 18),
(22, 19),
(23, 20),
(24, 21),
(25, 22),
(26, 23),
(27, 24),
(28, 25),
(29, 26),
(30, 27),
(31, 28),
(32, 29),
(33, 30),
(34, 31),
(35, 32),
(36, 33),
(37, 34),
(38, 35),
(39, 36),
(40, 37),
(41, 38),
(42, 39),
(43, 40),
(44, 41),
(45, 41),
(46, 42),
(47, 43),
(48, 44),
(49, 45);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `products`
--

CREATE TABLE `products` (
  `id` bigint UNSIGNED NOT NULL,
  `nombre` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `cantidad` decimal(5,2) NOT NULL,
  `kcalg` int NOT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Volcado de datos para la tabla `products`
--

INSERT INTO `products` (`id`, `nombre`, `cantidad`, `kcalg`, `created_at`, `updated_at`) VALUES
(1, 'guisantes', 594.00, 59, '2023-11-18 07:34:57', '2023-11-18 07:34:57');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `TipoAlimento`
--

CREATE TABLE `TipoAlimento` (
  `id_tipo` int NOT NULL,
  `kcal_g` float DEFAULT NULL,
  `prot_g` float DEFAULT NULL,
  `lip_g` float DEFAULT NULL,
  `carb_g` float DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `TipoAlimento`
--

INSERT INTO `TipoAlimento` (`id_tipo`, `kcal_g`, `prot_g`, `lip_g`, `carb_g`) VALUES
(1, 0.68406, 0.038015, 0.0379101, 0.047191),
(2, 0.465, 0.033, 0.016, 0.046),
(3, 0.391467, 0.0418222, 0.0016, 0.0521333),
(4, 0.841395, 0.0324837, 0.0206791, 0.131274),
(5, 1.13061, 0.0366531, 0.0269388, 0.18449),
(6, 0.455914, 0.00719086, 0.00174731, 0.102823),
(7, 0.22486, 0.0167374, 0.00312849, 0.0351955),
(8, 2.09587, 0.0630737, 0.0156211, 0.4256),
(9, 3.01, 0.219, 0.03, 0.465),
(10, 4.19395, 0.0711864, 0.211424, 0.501864),
(11, 6.51783, 0.0534783, 0.679565, 0.0456522),
(12, 0.87375, 0.0110625, 0.0879375, 0.00975),
(13, 3.6254, 0.0293651, 0.36746, 0.0492063),
(14, 6.08, 0.0272, 0.6624, 0.0032),
(15, 3.49137, 0.0210526, 0.0185263, 0.810947),
(16, 0.368903, 0.0738019, 0.00670927, 0.00255591),
(17, 0.73488, 0.11456, 0.03024, 0.00096),
(18, 1.70681, 0.153187, 0.113187, 0.00835165),
(19, 3.60191, 0.222766, 0.299362, 0.00446808),
(20, 3.41961, 0.145091, 0.308212, 0.0163333),
(21, 0.237069, 0.0176462, 0.00329835, 0.0371064),
(22, 1.17053, 0.0352263, 0.00872428, 0.237695),
(23, 1.11942, 0.0814463, 0.011157, 0.172934),
(24, 0.83694, 0.167436, 0.0152215, 0.00579866),
(25, 1.32127, 0.205973, 0.0543699, 0.00172603),
(26, 1.89704, 0.17026, 0.125802, 0.00928244);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `users`
--

CREATE TABLE `users` (
  `id` bigint UNSIGNED NOT NULL,
  `name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `email` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `email_verified_at` timestamp NULL DEFAULT NULL,
  `password` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `remember_token` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Usuario`
--

CREATE TABLE `Usuario` (
  `dni` varchar(9) NOT NULL,
  `nombre` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `Usuario`
--

INSERT INTO `Usuario` (`dni`, `nombre`) VALUES
('30987645F', 'Juan Pérez'),
('38042651J', 'Ana Marín');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `UsuarioMantel`
--

CREATE TABLE `UsuarioMantel` (
  `dni` varchar(9) NOT NULL,
  `id_mantel` int NOT NULL,
  `fecha_inicio` date NOT NULL,
  `fecha_fin` date DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `UsuarioMantel`
--

INSERT INTO `UsuarioMantel` (`dni`, `id_mantel`, `fecha_inicio`, `fecha_fin`) VALUES
('30987645F', 1, '2024-02-03', NULL),
('38042651J', 2, '2024-03-15', NULL);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `valores_nutricionales`
--

CREATE TABLE `valores_nutricionales` (
  `ID_grupo` int NOT NULL,
  `kcal_g` float DEFAULT NULL,
  `prot_g` float DEFAULT NULL,
  `lip_g` float DEFAULT NULL,
  `carb_g` float DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `valores_nutricionales`
--

INSERT INTO `valores_nutricionales` (`ID_grupo`, `kcal_g`, `prot_g`, `lip_g`, `carb_g`) VALUES
(1, 0.68406, 0.038015, 0.0379101, 0.047191),
(2, 0.465, 0.033, 0.016, 0.046),
(3, 0.391467, 0.0418222, 0.0016, 0.0521333),
(4, 0.841395, 0.0324837, 0.0206791, 0.131274),
(5, 1.13061, 0.0366531, 0.0269388, 0.18449),
(6, 0.455914, 0.00719086, 0.00174731, 0.102823),
(7, 0.22486, 0.0167374, 0.00312849, 0.0351955),
(8, 2.09587, 0.0630737, 0.0156211, 0.4256),
(9, 3.01, 0.219, 0.03, 0.465),
(10, 4.19395, 0.0711864, 0.211424, 0.501864),
(11, 6.51783, 0.0534783, 0.679565, 0.0456522),
(12, 0.87375, 0.0110625, 0.0879375, 0.00975),
(13, 3.6254, 0.0293651, 0.36746, 0.0492063),
(14, 6.08, 0.0272, 0.6624, 0.0032),
(15, 3.49137, 0.0210526, 0.0185263, 0.810947),
(16, 0.368903, 0.0738019, 0.00670927, 0.00255591),
(17, 0.73488, 0.11456, 0.03024, 0.00096),
(18, 1.70681, 0.153187, 0.113187, 0.00835165),
(19, 3.60191, 0.222766, 0.299362, 0.00446808),
(20, 3.41961, 0.145091, 0.308212, 0.0163333),
(27, 0.237069, 0.0176462, 0.00329835, 0.0371064),
(28, 1.17053, 0.0352263, 0.00872428, 0.237695),
(29, 1.11942, 0.0814463, 0.011157, 0.172934),
(36, 0.83694, 0.167436, 0.0152215, 0.00579866),
(37, 1.32127, 0.205973, 0.0543699, 0.00172603),
(38, 1.89704, 0.17026, 0.125802, 0.00928244);

--
-- Índices para tablas volcadas
--

--
-- Indices de la tabla `Alimento`
--
ALTER TABLE `Alimento`
  ADD PRIMARY KEY (`id_alimento`),
  ADD KEY `fk_Alimento_Plato` (`id_plato`),
  ADD KEY `fk_Alimento_Tipo` (`id_tipo`),
  ADD KEY `fk_Alimento_Usuario` (`dni`);

--
-- Indices de la tabla `alimento`
--
ALTER TABLE `alimento`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `plato_id` (`plato_id`);

--
-- Indices de la tabla `Barcode`
--
ALTER TABLE `Barcode`
  ADD PRIMARY KEY (`EAN`),
  ADD KEY `fk_Barcode_Tipo` (`id_tipo`);

--
-- Indices de la tabla `Comida`
--
ALTER TABLE `Comida`
  ADD PRIMARY KEY (`id_comida`);

--
-- Indices de la tabla `comida`
--
ALTER TABLE `comida`
  ADD PRIMARY KEY (`ID`);

--
-- Indices de la tabla `comidas`
--
ALTER TABLE `comidas`
  ADD PRIMARY KEY (`ID`);

--
-- Indices de la tabla `failed_jobs`
--
ALTER TABLE `failed_jobs`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `failed_jobs_uuid_unique` (`uuid`);

--
-- Indices de la tabla `GrupoAlimento`
--
ALTER TABLE `GrupoAlimento`
  ADD PRIMARY KEY (`grupo`),
  ADD KEY `fk_Grupo_Tipo` (`id_tipo`);

--
-- Indices de la tabla `IndiceGlucemico`
--
ALTER TABLE `IndiceGlucemico`
  ADD PRIMARY KEY (`id_medicion`);

--
-- Indices de la tabla `Insulina`
--
ALTER TABLE `Insulina`
  ADD PRIMARY KEY (`id_medicion`);

--
-- Indices de la tabla `Mantel`
--
ALTER TABLE `Mantel`
  ADD PRIMARY KEY (`id_mantel`);

--
-- Indices de la tabla `Medicion`
--
ALTER TABLE `Medicion`
  ADD PRIMARY KEY (`id_medicion`),
  ADD KEY `fk_Medicion_Usuario` (`dni`);

--
-- Indices de la tabla `migrations`
--
ALTER TABLE `migrations`
  ADD PRIMARY KEY (`id`);

--
-- Indices de la tabla `password_resets`
--
ALTER TABLE `password_resets`
  ADD KEY `password_resets_email_index` (`email`);

--
-- Indices de la tabla `personal_access_tokens`
--
ALTER TABLE `personal_access_tokens`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `personal_access_tokens_token_unique` (`token`),
  ADD KEY `personal_access_tokens_tokenable_type_tokenable_id_index` (`tokenable_type`,`tokenable_id`);

--
-- Indices de la tabla `plato`
--
ALTER TABLE `plato`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `comida_id` (`comida_id`);

--
-- Indices de la tabla `Plato`
--
ALTER TABLE `Plato`
  ADD PRIMARY KEY (`id_plato`),
  ADD KEY `fk_Plato_Comida` (`id_comida`);

--
-- Indices de la tabla `products`
--
ALTER TABLE `products`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `products_nombre_unique` (`nombre`);

--
-- Indices de la tabla `TipoAlimento`
--
ALTER TABLE `TipoAlimento`
  ADD PRIMARY KEY (`id_tipo`);

--
-- Indices de la tabla `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `users_email_unique` (`email`);

--
-- Indices de la tabla `Usuario`
--
ALTER TABLE `Usuario`
  ADD PRIMARY KEY (`dni`);

--
-- Indices de la tabla `UsuarioMantel`
--
ALTER TABLE `UsuarioMantel`
  ADD PRIMARY KEY (`dni`,`id_mantel`,`fecha_inicio`),
  ADD KEY `fk_Usuario_Mantel` (`id_mantel`);

--
-- Indices de la tabla `valores_nutricionales`
--
ALTER TABLE `valores_nutricionales`
  ADD PRIMARY KEY (`ID_grupo`);

--
-- AUTO_INCREMENT de las tablas volcadas
--

--
-- AUTO_INCREMENT de la tabla `Alimento`
--
ALTER TABLE `Alimento`
  MODIFY `id_alimento` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=80;

--
-- AUTO_INCREMENT de la tabla `alimento`
--
ALTER TABLE `alimento`
  MODIFY `ID` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=9;

--
-- AUTO_INCREMENT de la tabla `Comida`
--
ALTER TABLE `Comida`
  MODIFY `id_comida` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=46;

--
-- AUTO_INCREMENT de la tabla `comida`
--
ALTER TABLE `comida`
  MODIFY `ID` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT de la tabla `comidas`
--
ALTER TABLE `comidas`
  MODIFY `ID` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=51;

--
-- AUTO_INCREMENT de la tabla `failed_jobs`
--
ALTER TABLE `failed_jobs`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT de la tabla `Mantel`
--
ALTER TABLE `Mantel`
  MODIFY `id_mantel` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT de la tabla `Medicion`
--
ALTER TABLE `Medicion`
  MODIFY `id_medicion` int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT de la tabla `migrations`
--
ALTER TABLE `migrations`
  MODIFY `id` int UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT de la tabla `personal_access_tokens`
--
ALTER TABLE `personal_access_tokens`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT de la tabla `plato`
--
ALTER TABLE `plato`
  MODIFY `ID` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT de la tabla `Plato`
--
ALTER TABLE `Plato`
  MODIFY `id_plato` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=50;

--
-- AUTO_INCREMENT de la tabla `products`
--
ALTER TABLE `products`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT de la tabla `TipoAlimento`
--
ALTER TABLE `TipoAlimento`
  MODIFY `id_tipo` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=27;

--
-- AUTO_INCREMENT de la tabla `users`
--
ALTER TABLE `users`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- Restricciones para tablas volcadas
--

--
-- Filtros para la tabla `Alimento`
--
ALTER TABLE `Alimento`
  ADD CONSTRAINT `fk_Alimento_Plato` FOREIGN KEY (`id_plato`) REFERENCES `Plato` (`id_plato`),
  ADD CONSTRAINT `fk_Alimento_Tipo` FOREIGN KEY (`id_tipo`) REFERENCES `TipoAlimento` (`id_tipo`),
  ADD CONSTRAINT `fk_Alimento_Usuario` FOREIGN KEY (`dni`) REFERENCES `Usuario` (`dni`);

--
-- Filtros para la tabla `alimento`
--
ALTER TABLE `alimento`
  ADD CONSTRAINT `alimento_ibfk_1` FOREIGN KEY (`plato_id`) REFERENCES `plato` (`ID`);

--
-- Filtros para la tabla `Barcode`
--
ALTER TABLE `Barcode`
  ADD CONSTRAINT `fk_Barcode_Tipo` FOREIGN KEY (`id_tipo`) REFERENCES `TipoAlimento` (`id_tipo`);

--
-- Filtros para la tabla `GrupoAlimento`
--
ALTER TABLE `GrupoAlimento`
  ADD CONSTRAINT `fk_Grupo_Tipo` FOREIGN KEY (`id_tipo`) REFERENCES `TipoAlimento` (`id_tipo`);

--
-- Filtros para la tabla `IndiceGlucemico`
--
ALTER TABLE `IndiceGlucemico`
  ADD CONSTRAINT `fk_Glucemico_Medicion` FOREIGN KEY (`id_medicion`) REFERENCES `Medicion` (`id_medicion`);

--
-- Filtros para la tabla `Insulina`
--
ALTER TABLE `Insulina`
  ADD CONSTRAINT `fk_Insulina_Medicion` FOREIGN KEY (`id_medicion`) REFERENCES `Medicion` (`id_medicion`);

--
-- Filtros para la tabla `Medicion`
--
ALTER TABLE `Medicion`
  ADD CONSTRAINT `fk_Medicion_Usuario` FOREIGN KEY (`dni`) REFERENCES `Usuario` (`dni`);

--
-- Filtros para la tabla `plato`
--
ALTER TABLE `plato`
  ADD CONSTRAINT `plato_ibfk_1` FOREIGN KEY (`comida_id`) REFERENCES `comida` (`ID`);

--
-- Filtros para la tabla `Plato`
--
ALTER TABLE `Plato`
  ADD CONSTRAINT `fk_Plato_Comida` FOREIGN KEY (`id_comida`) REFERENCES `Comida` (`id_comida`);

--
-- Filtros para la tabla `UsuarioMantel`
--
ALTER TABLE `UsuarioMantel`
  ADD CONSTRAINT `fk_Mantel_Usuario` FOREIGN KEY (`dni`) REFERENCES `Usuario` (`dni`),
  ADD CONSTRAINT `fk_Usuario_Mantel` FOREIGN KEY (`id_mantel`) REFERENCES `Mantel` (`id_mantel`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
