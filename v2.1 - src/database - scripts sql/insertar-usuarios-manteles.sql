-- Insertar usuarios
INSERT INTO Usuario (dni, nombre) VALUES ('30987645F', 'Juan Pérez');
INSERT INTO Usuario (dni, nombre) VALUES ('38042651J', 'Ana Marín');

-- Insertar manteles
INSERT INTO Mantel (MAC, activo) VALUES ('08:D1:F9:CB:A1:EC', TRUE); -- Activo
INSERT INTO Mantel (MAC, activo) VALUES ('08:D1:F9:CB:4C:24', TRUE); -- Activo
INSERT INTO Mantel (MAC, activo) VALUES ('08:D1:F9:DE:F0:12', FALSE); -- No activo


-- Asignar manteles a usuarios
-- Primero, necesitamos obtener los id_mantel correspondientes a las MACs
SET @id_mantel = (SELECT id_mantel FROM Mantel WHERE MAC = '08:D1:F9:CB:A1:EC');
INSERT INTO UsuarioMantel (dni, id_mantel, fecha_inicio) VALUES ('30987645F', @id_mantel, '2024-02-03');

SET @id_mantel = (SELECT id_mantel FROM Mantel WHERE MAC = '08:D1:F9:CB:4C:24');
INSERT INTO UsuarioMantel (dni, id_mantel, fecha_inicio) VALUES ('38042651J', @id_mantel, '2024-03-15');