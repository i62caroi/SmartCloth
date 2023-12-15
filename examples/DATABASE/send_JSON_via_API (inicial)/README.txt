URL para POST:

http://smartcloth.site/post-esp32-data-json.php




URL para GET:

http://smartcloth.site/esp32-data-json.php




Se envía la api_key en el cuerpo del JSON, pero no es seguro. 
Habrá que cambiarlo para enviarla en el header del POST.




TABLAS:

"comida":
	- ID (autoincremental)
	- fecha 
	- hora

"plato":
	- ID (autoincremental)
	- comida_id (ID de la comida en tabla "comida" a la que pertenece el plato)

"alimento":
	- ID (autoincremental)
	- grupo
	- peso
	- plato_id (ID del plato en tabla "plato" al que pertenece el alimento)
