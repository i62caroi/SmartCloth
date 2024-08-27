-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Servidor: 192.168.0.100
-- Tiempo de generación: 14-07-2024 a las 20:07:29
-- Versión del servidor: 8.0.36-28
-- Versión de PHP: 7.3.13

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET FOREIGN_KEY_CHECKS = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de datos: `smartclo_SM-database`
--

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `alimentos`
--

CREATE TABLE `alimentos` (
  `id` int NOT NULL,
  `peso` float NOT NULL,
  `plato_id` int NOT NULL,
  `tipoalimento_id` int NOT NULL,
  `user_id` varchar(9) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `alimentos`
--

INSERT INTO `alimentos` (`id`, `peso`, `plato_id`, `tipoalimento_id`, `user_id`) VALUES
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
(79, 24.4, 49, 15, '38042651J'),
(80, 53.5, 50, 15, '30987645F'),
(81, 53.5, 50, 9, '30987645F'),
(82, 24.4, 51, 15, '30987645F'),
(83, 53.5, 52, 15, '30987645F'),
(84, 53.5, 52, 9, '30987645F'),
(85, 24.4, 53, 15, '30987645F'),
(86, 53.5, 54, 15, '30987645F'),
(87, 53.5, 54, 9, '30987645F'),
(88, 24.4, 55, 15, '30987645F'),
(89, 53.5, 56, 15, '30987645F'),
(90, 53.5, 56, 9, '30987645F'),
(91, 24.4, 57, 15, '30987645F'),
(92, 53.5, 58, 15, '30987645F'),
(93, 53.5, 58, 9, '30987645F'),
(94, 150.7, 58, 27, '30987645F'),
(95, 24.4, 59, 13, '30987645F'),
(96, 53.5, 60, 15, '30987645F'),
(97, 53.5, 60, 9, '30987645F'),
(98, 24.4, 61, 15, '30987645F'),
(99, 53.5, 62, 15, '30987645F'),
(100, 53.5, 62, 9, '30987645F'),
(101, 24.4, 63, 15, '30987645F'),
(102, 53.5, 64, 15, '30987645F'),
(103, 53.5, 64, 9, '30987645F'),
(104, 120.5, 64, 27, '30987645F'),
(105, 53.5, 65, 5, '30987645F'),
(106, 23.5, 65, 7, '30987645F'),
(107, 120.5, 65, 28, '30987645F'),
(108, 53.5, 66, 9, '38042651J'),
(109, 43.5, 67, 21, '38042651J'),
(110, 53.5, 67, 9, '38042651J'),
(111, 43.5, 68, 21, '38042651J'),
(112, 53.5, 68, 9, '38042651J'),
(113, 43.5, 69, 21, '38042651J'),
(114, 53.5, 69, 9, '38042651J'),
(115, 43.5, 70, 21, '38042651J'),
(116, 53.5, 70, 9, '38042651J'),
(117, 33.8, 70, 29, '38042651J'),
(118, 43.5, 71, 21, '38042651J'),
(119, 53.5, 71, 9, '38042651J'),
(120, 33.8, 71, 29, '38042651J'),
(121, 43.5, 72, 21, '38042651J'),
(122, 53.5, 72, 9, '38042651J');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `barcodes`
--

CREATE TABLE `barcodes` (
  `ean` varchar(13) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `nombre` varchar(100) DEFAULT NULL,
  `tipoalimento_id` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `barcodes`
--

INSERT INTO `barcodes` (`ean`, `nombre`, `tipoalimento_id`) VALUES
('5601560111905', 'Mini tostadas de trigo extra finas', 27),
('8424621001093', 'Reloj', 28),
('8480000640666', 'Mini almendrado', 29);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `comidas`
--

CREATE TABLE `comidas` (
  `id` int NOT NULL,
  `fecha` timestamp NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `comidas`
--

INSERT INTO `comidas` (`id`, `fecha`) VALUES
(1, '2005-12-20 19:00:00'),
(2, '2005-12-20 19:00:00'),
(3, '2020-12-20 17:00:00'),
(4, '2005-12-20 19:00:00'),
(5, '2020-12-20 17:00:00'),
(6, '2020-12-20 17:00:00'),
(7, '2005-12-20 19:00:00'),
(8, '2005-12-20 19:00:00'),
(9, '2005-12-20 19:00:00'),
(10, '2005-12-20 19:00:00'),
(11, '2005-12-20 19:00:00'),
(12, '2005-12-20 19:00:00'),
(13, '2005-12-20 19:00:00'),
(14, '2005-12-20 19:00:00'),
(15, '2005-12-20 19:00:00'),
(16, '2005-12-20 19:00:00'),
(17, '2005-12-19 22:03:00'),
(18, '2005-12-20 19:00:00'),
(19, '2005-12-20 19:00:00'),
(20, '2005-12-20 19:00:00'),
(21, '2005-12-20 19:00:00'),
(22, '2005-12-20 01:00:00'),
(23, '2005-12-20 19:00:00'),
(24, '2005-12-20 19:00:00'),
(25, '2005-12-20 19:00:00'),
(26, '2005-12-20 19:00:00'),
(27, '2005-12-20 19:00:00'),
(28, '2005-12-20 19:00:00'),
(29, '2024-01-19 14:55:36'),
(30, '2024-01-19 15:25:25'),
(31, '2024-01-19 12:55:36'),
(32, '2024-01-19 13:25:25'),
(33, '2024-01-19 11:55:36'),
(34, '2024-01-19 12:25:25'),
(35, '2024-01-19 11:55:36'),
(36, '2024-01-19 12:25:25'),
(37, '2024-01-19 11:55:36'),
(38, '2024-01-19 12:25:25'),
(39, '2024-01-19 11:55:36'),
(40, '2024-01-19 12:25:25'),
(41, '2024-01-19 04:39:36'),
(42, '2023-12-05 11:55:36'),
(43, '2023-12-05 12:25:25'),
(44, '2023-12-05 11:55:36'),
(45, '2023-12-05 12:25:25'),
(46, '2024-05-02 14:55:36'),
(47, '2024-05-02 15:25:25'),
(48, '2024-05-02 14:55:36'),
(49, '2024-05-02 15:25:25'),
(50, '2024-05-02 14:55:36'),
(51, '2024-05-02 15:25:25'),
(52, '2024-05-02 14:55:36'),
(53, '2024-05-02 15:25:25'),
(54, '2024-07-04 09:28:51'),
(55, '2024-07-04 16:19:51'),
(56, '2024-05-02 14:55:36'),
(57, '2024-05-02 15:25:25'),
(58, '2024-05-02 14:55:36'),
(59, '2024-05-02 15:25:25'),
(60, '2024-05-02 14:55:36'),
(61, '2024-07-03 05:55:36'),
(62, '2024-07-14 12:40:29'),
(63, '2024-07-14 12:45:54'),
(64, '2024-07-14 12:54:35'),
(65, '2024-07-14 12:59:24'),
(66, '2024-07-14 13:02:09'),
(67, '2024-07-14 13:41:26'),
(68, '2024-07-14 14:04:47');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `grupos`
--

CREATE TABLE `grupos` (
  `cod_grupo` int NOT NULL,
  `nombre` varchar(255) DEFAULT NULL,
  `ejemplos` text,
  `tipoalimento_id` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `grupos`
--

INSERT INTO `grupos` (`cod_grupo`, `nombre`, `ejemplos`, `tipoalimento_id`) VALUES
(1, 'Lácteos enteros', 'Leche entera de vaca (pasteurizada o UHT), de oveja, de cabra, yogurt natural entero, cuajada, etc.', 1),
(2, 'Lácteos desnatados', 'Leche desnatada, yogurt natural desnatado, etc.', 2),
(3, 'Lácteos desnatados', 'Leche desnatada pasteurizada y UHT, natural, con frutas, yogurt desnatado, yogurt desnatado de sabores, etc.', 3),
(4, 'Lácteos azucarados', 'Batidos lácteos de cacao y otros sabores, leche entera fermentada con frutas, yogures enteros de sabores y azucarados, yogures líquidos de sabores y azucarados', 4),
(5, 'Postres lácteos', 'Arroz con leche, flan de huevo, flan de vainilla y natillas...', 5),
(6, 'Frutas frescas, desecadas y zumos', 'Albaricoque, arándanos, cerezas, ciruelas, dátil seco, fresa, granada, higos, kiwi, mandarina,manzana, melocotón, melón, naranja, pera, piña, plátano, sandía, uvas...', 6),
(7, 'Verduras y hortalizas', 'Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza, champiñones, col, espárragos, espinacas, guisantes, lechuga, judías, pimientos, tomate, zanahoria, etc.', 7),
(8, 'Cereales y tubérculos', 'Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra, copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.', 8),
(9, 'Legumbres', 'Alubias, garbanzos, lentejas, etc.', 9),
(10, 'Repostería, pastelería y otros', 'Bizcocho, bollo, croissant, ensaimada, galletas (de cualquier tipo), magdalena, muesli, pan de pasas, tartas, pasteles, bollería industrial...', 10),
(11, 'Alimentos ricos en grasas saludables', 'Aceites de cacahuete, de oliva y de hígado de bacalao, aceitunas, aguacate, almendras, avellanas, cacahuetes, pistachos, mayonesa de aceite de oliva, etc.', 11),
(12, 'Alimentos ricos en grasas vegetales', 'Aceite de girasol, aceite de maíz, aceite de soja, mayonesa light, nueces, piñones, etc.', 12),
(13, 'Alimentos ricos en grasas saturadas', 'Coco fresco o seco, aceite de coco, mantequilla, nata líquida para cocinar o montar, etc.', 13),
(14, 'Alimentos muy grasos (mezclas)', 'Margarina light, margarina vegetal enriquecida, manteca y tocino de cerdo', 14),
(15, 'Azúcares y dulces', 'Azúcar blanco y moreno, miel, leche condensada, cacao soluble azucarado, confitura de fruta baja en calorías', 15),
(16, 'Alimentos proteicos con muy poca grasa', 'Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural, pescado no graso, marisco, queso granulado, clara de huevo, etc.', 16),
(17, 'Alimentos proteicos con poca grasa', 'Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato sin piel, codorniz, etc.', 17),
(18, 'Alimentos proteicos semigrasos', 'Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa, salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en porciones, etc.', 18),
(19, 'Alimentos proteicos grasos', 'Chuletas/costillas de cordero, chorizo, salchichas, fuet, quesos (azul, babybel, camembert, cheddar, de cabra, emmental, gouda, gruyer, manchego...)', 19),
(20, 'Alimentos proteicos muy grasos', 'Carne picada sazonada, panceta de cerdo, morcilla, mortadela, paté, salami, salchichón, etc.', 20),
(27, 'Verduras y hortalizas', 'Acelgas, apio, alcachofa, berenjena, brócoli, calabacín, calabaza, champiñones, col, espárragos, espinacas, guisantes, lechuga, judías, pimientos, tomate, zanahoria, etc.', 21),
(28, 'Cereales y tubérculos', 'Arroz, avena, boniato, castaña, cereales de desayuno ricos en fibra, copos de maíz, harina, maíz, pan, pasta, patata, sémola de trigo, etc.', 22),
(29, 'Legumbres', 'Alubias, garbanzos, lentejas, etc.', 23),
(36, 'Alimentos proteicos con muy poca grasa', 'Pavo, pollo, ternera (entrecot y solomillo), jamón cocido, atún natural, pescado no graso, marisco, queso granulado, clara de huevo, etc.', 24),
(37, 'Alimentos proteicos con poca grasa', 'Lomo de cerdo, pollo sin piel, bistec de vaca/buey, jamón curado (sin grasa), pescados grasos (atún, sardina, trucha, boquerón...), vísceras, pato sin piel, codorniz, etc.', 25),
(38, 'Alimentos proteicos semigrasos', 'Chuletas de cerdo, cordero, anchoas, atún o sardinas en aceite, caballa, salmón, jamón curado con grasa, huevo, queso fresco, requesón, queso en porciones, etc.', 26);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `ind_glucemicos`
--

CREATE TABLE `ind_glucemicos` (
  `medicion_id` int NOT NULL,
  `valor` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `insulinas`
--

CREATE TABLE `insulinas` (
  `medicion_id` int NOT NULL,
  `valor` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `mantels`
--

CREATE TABLE `mantels` (
  `id` int NOT NULL,
  `mac` varchar(17) NOT NULL,
  `activo` tinyint(1) NOT NULL DEFAULT '0',
  `user_id` varchar(9) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `mantels`
--

INSERT INTO `mantels` (`id`, `mac`, `activo`, `user_id`) VALUES
(1, '3C:E9:0E:7C:3B:60', 1, '30987645F'),
(2, '08:D1:F9:CB:4C:24', 1, '38042651J'),
(3, '08:D1:F9:DE:F0:12', 0, NULL);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `medicions`
--

CREATE TABLE `medicions` (
  `id` int NOT NULL,
  `user_id` varchar(9) NOT NULL,
  `fecha` timestamp NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `platos`
--

CREATE TABLE `platos` (
  `id` int NOT NULL,
  `comida_id` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `platos`
--

INSERT INTO `platos` (`id`, `comida_id`) VALUES
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
(49, 45),
(50, 46),
(51, 47),
(52, 48),
(53, 49),
(54, 50),
(55, 51),
(56, 52),
(57, 53),
(58, 54),
(59, 55),
(60, 56),
(61, 57),
(62, 58),
(63, 59),
(64, 60),
(65, 61),
(66, 62),
(67, 63),
(68, 64),
(69, 65),
(70, 66),
(71, 67),
(72, 68);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `tipo_alimentos`
--

CREATE TABLE `tipo_alimentos` (
  `id` int NOT NULL,
  `kcal_g` float DEFAULT NULL,
  `prot_g` float DEFAULT NULL,
  `lip_g` float DEFAULT NULL,
  `carb_g` float DEFAULT NULL,
  `tipo` varchar(8) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `tipo_alimentos`
--

INSERT INTO `tipo_alimentos` (`id`, `kcal_g`, `prot_g`, `lip_g`, `carb_g`, `tipo`) VALUES
(1, 0.68406, 0.038015, 0.0379101, 0.047191, 'grupo'),
(2, 0.465, 0.033, 0.016, 0.046, 'grupo'),
(3, 0.391467, 0.0418222, 0.0016, 0.0521333, 'grupo'),
(4, 0.841395, 0.0324837, 0.0206791, 0.131274, 'grupo'),
(5, 1.13061, 0.0366531, 0.0269388, 0.18449, 'grupo'),
(6, 0.455914, 0.00719086, 0.00174731, 0.102823, 'grupo'),
(7, 0.22486, 0.0167374, 0.00312849, 0.0351955, 'grupo'),
(8, 2.09587, 0.0630737, 0.0156211, 0.4256, 'grupo'),
(9, 3.01, 0.219, 0.03, 0.465, 'grupo'),
(10, 4.19395, 0.0711864, 0.211424, 0.501864, 'grupo'),
(11, 6.51783, 0.0534783, 0.679565, 0.0456522, 'grupo'),
(12, 0.87375, 0.0110625, 0.0879375, 0.00975, 'grupo'),
(13, 3.6254, 0.0293651, 0.36746, 0.0492063, 'grupo'),
(14, 6.08, 0.0272, 0.6624, 0.0032, 'grupo'),
(15, 3.49137, 0.0210526, 0.0185263, 0.810947, 'grupo'),
(16, 0.368903, 0.0738019, 0.00670927, 0.00255591, 'grupo'),
(17, 0.73488, 0.11456, 0.03024, 0.00096, 'grupo'),
(18, 1.70681, 0.153187, 0.113187, 0.00835165, 'grupo'),
(19, 3.60191, 0.222766, 0.299362, 0.00446808, 'grupo'),
(20, 3.41961, 0.145091, 0.308212, 0.0163333, 'grupo'),
(21, 0.237069, 0.0176462, 0.00329835, 0.0371064, 'grupo'),
(22, 1.17053, 0.0352263, 0.00872428, 0.237695, 'grupo'),
(23, 1.11942, 0.0814463, 0.011157, 0.172934, 'grupo'),
(24, 0.83694, 0.167436, 0.0152215, 0.00579866, 'grupo'),
(25, 1.32127, 0.205973, 0.0543699, 0.00172603, 'grupo'),
(26, 1.89704, 0.17026, 0.125802, 0.00928244, 'grupo'),
(27, 3.84, 0.118, 0.024, 0.769, 'barcode'),
(28, 3.8, 0, 0, 0.96, 'barcode'),
(29, 3.1, 0.046, 0.19, 0.293, 'barcode');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `users`
--

CREATE TABLE `users` (
  `id` varchar(9) NOT NULL,
  `name` varchar(100) NOT NULL,
  `fecha_inicio` date NOT NULL,
  `fecha_fin` date DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Volcado de datos para la tabla `users`
--

INSERT INTO `users` (`id`, `name`, `fecha_inicio`, `fecha_fin`) VALUES
('30987645F', 'Juan Pérez', '2024-02-03', NULL),
('38042651J', 'Ana Marín', '2024-03-15', NULL);

--
-- Índices para tablas volcadas
--

--
-- Indices de la tabla `alimentos`
--
ALTER TABLE `alimentos`
  ADD PRIMARY KEY (`id`),
  ADD KEY `fk_Alimento_Plato` (`plato_id`),
  ADD KEY `fk_Alimento_Tipo` (`tipoalimento_id`),
  ADD KEY `fk_Alimento_Usuario` (`user_id`);

--
-- Indices de la tabla `barcodes`
--
ALTER TABLE `barcodes`
  ADD PRIMARY KEY (`ean`),
  ADD KEY `fk_Barcode_Tipo` (`tipoalimento_id`);

--
-- Indices de la tabla `comidas`
--
ALTER TABLE `comidas`
  ADD PRIMARY KEY (`id`);

--
-- Indices de la tabla `grupos`
--
ALTER TABLE `grupos`
  ADD PRIMARY KEY (`cod_grupo`),
  ADD KEY `fk_Grupo_Tipo` (`tipoalimento_id`);

--
-- Indices de la tabla `ind_glucemicos`
--
ALTER TABLE `ind_glucemicos`
  ADD PRIMARY KEY (`medicion_id`);

--
-- Indices de la tabla `insulinas`
--
ALTER TABLE `insulinas`
  ADD PRIMARY KEY (`medicion_id`);

--
-- Indices de la tabla `mantels`
--
ALTER TABLE `mantels`
  ADD PRIMARY KEY (`id`),
  ADD KEY `fk_Alimento_Usuario` (`user_id`);

--
-- Indices de la tabla `medicions`
--
ALTER TABLE `medicions`
  ADD PRIMARY KEY (`id`),
  ADD KEY `fk_Medicion_Usuario` (`user_id`);

--
-- Indices de la tabla `platos`
--
ALTER TABLE `platos`
  ADD PRIMARY KEY (`id`),
  ADD KEY `fk_Plato_Comida` (`comida_id`);

--
-- Indices de la tabla `tipo_alimentos`
--
ALTER TABLE `tipo_alimentos`
  ADD PRIMARY KEY (`id`);

--
-- Indices de la tabla `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT de las tablas volcadas
--

--
-- AUTO_INCREMENT de la tabla `alimentos`
--
ALTER TABLE `alimentos`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=124;

--
-- AUTO_INCREMENT de la tabla `comidas`
--
ALTER TABLE `comidas`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=69;

--
-- AUTO_INCREMENT de la tabla `mantels`
--
ALTER TABLE `mantels`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT de la tabla `medicions`
--
ALTER TABLE `medicions`
  MODIFY `id` int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT de la tabla `platos`
--
ALTER TABLE `platos`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=73;

--
-- AUTO_INCREMENT de la tabla `tipo_alimentos`
--
ALTER TABLE `tipo_alimentos`
  MODIFY `id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=35;

--
-- Restricciones para tablas volcadas
--

--
-- Filtros para la tabla `alimentos`
--
ALTER TABLE `alimentos`
  ADD CONSTRAINT `fk_Alimento_Plato` FOREIGN KEY (`plato_id`) REFERENCES `platos` (`id`),
  ADD CONSTRAINT `fk_Alimento_Tipo` FOREIGN KEY (`tipoalimento_id`) REFERENCES `tipo_alimentos` (`id`),
  ADD CONSTRAINT `fk_Alimento_Usuario` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`);

--
-- Filtros para la tabla `barcodes`
--
ALTER TABLE `barcodes`
  ADD CONSTRAINT `fk_Barcode_Tipo` FOREIGN KEY (`tipoalimento_id`) REFERENCES `tipo_alimentos` (`id`);

--
-- Filtros para la tabla `grupos`
--
ALTER TABLE `grupos`
  ADD CONSTRAINT `fk_Grupo_Tipo` FOREIGN KEY (`tipoalimento_id`) REFERENCES `tipo_alimentos` (`id`);

--
-- Filtros para la tabla `ind_glucemicos`
--
ALTER TABLE `ind_glucemicos`
  ADD CONSTRAINT `fk_Glucemico_Medicion` FOREIGN KEY (`medicion_id`) REFERENCES `medicions` (`id`);

--
-- Filtros para la tabla `insulinas`
--
ALTER TABLE `insulinas`
  ADD CONSTRAINT `fk_Insulina_Medicion` FOREIGN KEY (`medicion_id`) REFERENCES `medicions` (`id`);

--
-- Filtros para la tabla `mantels`
--
ALTER TABLE `mantels`
  ADD CONSTRAINT `fk_Mantel_Usuario` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`);

--
-- Filtros para la tabla `medicions`
--
ALTER TABLE `medicions`
  ADD CONSTRAINT `fk_Medicion_Usuario` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`);

--
-- Filtros para la tabla `platos`
--
ALTER TABLE `platos`
  ADD CONSTRAINT `fk_Plato_Comida` FOREIGN KEY (`comida_id`) REFERENCES `comidas` (`id`);

COMMIT;

SET FOREIGN_KEY_CHECKS = 1;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
