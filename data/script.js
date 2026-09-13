// ==================================================
// CARGAR REDES WIFI
// ==================================================

function cargarRedes()
{
    fetch("/redes")
        .then(response => response.json())
        .then(redes =>
        {
            const listaRedes =
                document.getElementById("redes");

            listaRedes.innerHTML = "";

            if (redes.length === 0)
            {
                const opcion =
                    document.createElement("option");

                opcion.textContent =
                    "No se encontraron redes";

                listaRedes.appendChild(opcion);

                return;
            }

            redes.forEach(red =>
            {
                const opcion =
                    document.createElement("option");

                opcion.value = red.ssid;

                opcion.textContent =
                    red.ssid + " (" + red.rssi + " dBm)";

                listaRedes.appendChild(opcion);
            });
        })
        .catch(error =>
        {
            console.log(
                "Error buscando redes:",
                error
            );
        });
}


// ==================================================
// MOSTRAR / OCULTAR CONTRASEÑA
// ==================================================

function mostrarOcultarPassword()
{
    const password =
        document.getElementById("password");

    const boton =
        document.getElementById("mostrarPassword");

    if (password.type === "password")
    {
        password.type = "text";

        boton.textContent = "🙈";
    }
    else
    {
        password.type = "password";

        boton.textContent = "👁️";
    }
}


// ==================================================
// ENVIAR CONFIGURACIÓN WIFI
// ==================================================

document
    .getElementById("wifiForm")
    .addEventListener("submit", function(event)
    {
        event.preventDefault();

        const ssid =
            document.getElementById("redes").value;

        const password =
            document.getElementById("password").value;

        const estado =
            document.getElementById("estado");

        estado.textContent =
            "Conectando...";


        fetch("/configurar",
        {
            method: "POST",

            headers:
            {
                "Content-Type":
                    "application/x-www-form-urlencoded"
            },

            body:
                "ssid=" +
                encodeURIComponent(ssid) +
                "&password=" +
                encodeURIComponent(password)
        })

        .then(response =>
        {
            return response.text();
        })

        .then(resultado =>
        {
            estado.textContent =
                resultado;
        })

        .catch(error =>
        {
            console.log(
                "Error conectando:",
                error
            );

            estado.textContent =
                "Error de conexión";
        });
    });


// ==================================================
// ACTUALIZAR ESTADO WIFI
// ==================================================

function actualizarEstado()
{
    fetch("/estado")
        .then(response => response.json())
        .then(datos =>
        {
            const estadoConexion =
                document.getElementById(
                    "estadoConexion"
                );

            const nombreRed =
                document.getElementById(
                    "nombreRed"
                );

            const direccionIP =
                document.getElementById(
                    "direccionIP"
                );

            const nivelSenal =
                document.getElementById(
                    "nivelSenal"
                );


            // ------------------------------------------
            // ESP8266 CONECTADO
            // ------------------------------------------

            if (datos.conectado)
            {
                estadoConexion.textContent =
                    "✓ Conectado";

                nombreRed.textContent =
                    datos.ssid;

                direccionIP.textContent =
                    datos.ip;

                nivelSenal.textContent =
                    datos.rssi + " dBm";

                actualizarIconoWifi(datos.rssi);
            }


            // ------------------------------------------
            // ESP8266 NO CONECTADO
            // ------------------------------------------

            else
            {
                estadoConexion.textContent =
                    "⚠ No conectado";

                nombreRed.textContent =
                    "Punto de acceso";

                direccionIP.textContent =
                    datos.ip;

                nivelSenal.textContent =
                    "-";

                actualizarBarras(-100);
            }
        })

        .catch(error =>
        {
            console.log(
                "Error obteniendo estado:",
                error
            );
        });
}


// ==================================================
// ACTUALIZAR BARRAS DE SEÑAL
// ==================================================

function actualizarBarras(rssi)
{
    const nivel =
        obtenerNivelSenal(rssi);

    const barra1 =
        document.getElementById("barra1");

    const barra2 =
        document.getElementById("barra2");

    const barra3 =
        document.getElementById("barra3");

    const barra4 =
        document.getElementById("barra4");


    // Apagar todas las barras

    barra1.style.background = "#ccc";
    barra2.style.background = "#ccc";
    barra3.style.background = "#ccc";
    barra4.style.background = "#ccc";


    // Encender las necesarias

    if (nivel >= 1)
    {
        barra1.style.background = "#000";
    }

    if (nivel >= 2)
    {
        barra2.style.background = "#000";
    }

    if (nivel >= 3)
    {
        barra3.style.background = "#000";
    }

    if (nivel >= 4)
    {
        barra4.style.background = "#000";
    }
}

function obtenerNivelSenal(rssi)
{
    if (rssi >= -50)
    {
        return 4;
    }

    if (rssi >= -60)
    {
        return 3;
    }

    if (rssi >= -70)
    {
        return 2;
    }

    if (rssi >= -80)
    {
        return 1;
    }

    return 0;
}

function actualizarIconoWifi(rssi)
{
    const nivel =
        obtenerNivelSenal(rssi);

    const arco3 =
        document.getElementById("wifiArco3");

    const arco2 =
        document.getElementById("wifiArco2");

    const arco1 =
        document.getElementById("wifiArco1");

    const punto =
        document.getElementById("wifiPunto");


    // Apagar todo

    arco3.style.borderColor = "#ccc";
    arco2.style.borderColor = "#ccc";
    arco1.style.borderColor = "#ccc";
    punto.style.background = "#ccc";


    // Nivel 1

    if (nivel >= 1)
    {
        punto.style.background = "#000";
    }


    // Nivel 2

    if (nivel >= 2)
    {
        arco1.style.borderColor = "#000";
        arco1.style.borderBottomColor = "transparent";
        arco1.style.borderLeftColor = "transparent";
    }


    // Nivel 3

    if (nivel >= 3)
    {
        arco2.style.borderColor = "#000";
        arco2.style.borderBottomColor = "transparent";
        arco2.style.borderLeftColor = "transparent";
    }


    // Nivel 4

    if (nivel >= 4)
    {
        arco3.style.borderColor = "#000";
        arco3.style.borderBottomColor = "transparent";
        arco3.style.borderLeftColor = "transparent";
    }
}

// ==================================================
// INICIALIZACIÓN
// ==================================================

// Cargar las redes inmediatamente
cargarRedes();

// Obtener el estado inmediatamente
actualizarEstado();

// Actualizar el estado cada 5 segundos
setInterval(
    actualizarEstado,
    5000
);