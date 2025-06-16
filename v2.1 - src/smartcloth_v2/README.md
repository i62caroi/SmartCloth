# Gestión de retirada inesperada de plato

## Problema
Los usuarios a menudo retiran el plato sin indicar previamente qué acción deseaban realizar (añadir otro plato, eliminar o guardar comida), causando:
- Pérdida de datos del último alimento
- Estados inconsistentes
- Experiencia confusa

Esto puede ocurrir en diferentes escenarios:

1. **Al cambiar de plato**: El usuario quiere cambiar a un nuevo plato, pero retira el actual sin usar el botón "Añadir plato".

2. **Al eliminar plato**: El usuario quiere eliminar el plato actual, pero lo retira sin usar el botón "Borrar plato".

3. **Al finalizar una comida**: El usuario ha terminado, pero en lugar de seleccionar "Guardar" directamente retira el plato. Este caso no es tan común, pero también posible.

## Solución implementada
Se ha creado un sistema que:
1. **Detecta** automáticamente cuando se retira el plato sin aviso previo
2. **Consulta** al usuario sobre su intención (añadir otro plato, eliminar o guardar comida)
3. **Procede** con la acción seleccionada, o elimina el plato tras 30 segundos sin respuesta
4. **Mantiene** la coherencia de la interfaz a través de todos los posibles flujos

## Flujos de recuperación
- **Sin respuesta del usuario**: Eliminación automática tras 30 segundos
- **El usuario selecciona una acción**: Se procesa adecuadamente (añadir/eliminar/guardar)
- **Ocurre un error durante la acción**: El sistema mantiene el contexto y vuelve a un estado coherente

## Limitaciones
- El último alimento colocado no puede guardarse si se retira el plato sin avisar
- La pantalla de consulta solo aparece si el plato contenía alimentos

## Mejoras futuras
- Almacenamiento temporal del peso del último alimento
- Ajuste del tiempo de espera basado en feedback de usuarios
- Soporte para casos de uso adicionales