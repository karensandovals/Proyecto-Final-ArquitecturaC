# Proyecto-Final-ArquitecturaC
# Sistema de control de acceso y monitoreo con Arduino

Se implementa un sistema de control de acceso y monitoreo utilizando una placa Arduino.

![image](https://github.com/karensandovals/Proyecto-Final-ArquitecturaC/assets/136817019/d86befa4-1c00-49f2-88e5-9ec2e2c40b3a)  ![image](https://github.com/karensandovals/Proyecto-Final-ArquitecturaC/assets/136817019/0278b71a-a7be-47c5-aa2b-e6fe1549ed3b)

El sistema permite:

Ingreso de usuarios: Un usuario puede ingresar al sistema utilizando una clave secreta.
Menú principal: Una vez ingresado, el usuario tiene acceso a un menú principal donde puede:
Monitorear sensores: El sistema puede leer datos de sensores como temperatura, humedad y luminosidad y mostrarlos en la pantalla LCD.
Visualizar alarmas: Si se detecta un valor fuera de rango en los sensores, el sistema activa una alarma visual (LED rojo) y sonora (buzzer) y muestra un mensaje en la pantalla LCD.
Bloqueo por intentos fallidos: Si un usuario ingresa la clave incorrecta varias veces, el sistema se bloquea y requiere un reinicio.

# Bosquejo de sistema
![image](https://github.com/karensandovals/Proyecto-Final-ArquitecturaC/assets/136817019/d14f30aa-96f5-46f4-8c2d-beb0af6dbc8e)
