# API VIAJES - Documentación

## Descripción General
APIs para la gestión del ciclo de vida completo de viajes programados y en ejecución.

??? info "Notas Importantes"
    - Sistema de gestión dual: viajes programados (ScheduledTrip) y viajes en ejecución (ActualTrip)
    - Manejo de zonas horarias entre UTC y hora local de Chile
    - Sistema de estados para seguimiento de ejecución del viaje
    - Integración con WebPubSub para notificaciones en tiempo real
    - Validaciones geográficas para confirmación de ubicaciones
    - Control de acceso basado en JWT
    - Registro detallado de eventos y cambios de estado

## Endpoint Post Trip
`POST /api/logipath/post_trip`

=== "Flujo Explicado"
    ### 1. Verificación Inicial
    - `Validación del token JWT`
    - `Normalización de timestamps (eliminación de microsegundos)`
    - `Verificación de viaje existente`

    ### 2. Actualización de Viaje Existente
    - `Si existe HoraConfirmacion:`
    - `Busca el último viaje del conductor`
    - `Actualiza campos no nulos del request`
    - `Maneja estados especiales (finalización/cancelación)`

    ### 3. Creación de Nuevo Viaje
    - `Si no hay HoraConfirmacion:`
    - `Crea nuevo ActualTrip`
    - `Asigna valores iniciales`
    - `Maneja estados de cancelación si aplica`

    ### 4. Notificaciones
    - `Envío de actualizaciones vía WebPubSub`
    - `Actualización de estados en ScheduledTrip relacionado`

    ```json title="Estructura de API viajes" linenums="1" hl_lines="3-5"
    {
        "Id": "string?",
        "IdProgramado": "number",
        "RutConductor": "string",
        "HoraInicioJornada": "datetime",
        "PlantaOrigen": "string?",
        "Cliente": "string?",
        "Patente": "string?",
        "LatitudInicioJornada": "number?",
        "LongitudInicioJornada": "number?",
        "HoraConfirmacion": "datetime?",
        "TaraViaje": "number?",
        "HoraInicioCarga": "datetime?",
        "LatitudInicioCarga": "number?",
        "LongitudInicioCarga": "number?",
        "HoraFinCarga": "datetime?",
        "LatitudFinCarga": "number?",
        "LongitudFinCarga": "number?",
        "HoraPesaje": "datetime?",
        "LatitudPesaje": "number?",
        "LongitudPesaje": "number?",
        "Folio": "number?",
        "HoraConfirmacionFolio": "datetime?",
        "LongitudConFolio": "number?",
        "LatitudConFolio": "number?",
        "CantidadPausa": "boolean?",
        "HoraLlegadaCliente": "datetime?",
        "LatitudLlegadaCliente": "number?",
        "LongitudLlegadaCliente": "number?",
        "HoraFinEntrega": "datetime?",
        "LatitudFinEntrega": "number?",
        "LongitudFinEntrega": "number?",
        "IdTicket": "string?",
        "PesoConfirmadoManual": "number?",
        "Cancelado": "boolean?",
        "Comentario": "string?",
        "Version": "string?"
    }
    ```
=== "Estados y Respuestas"
    ## Estados del Viaje
    - `Inicio Jornada`
    - `Confirmación`
    - `Carga`
    - `Pesaje`
    - `Con Folio`
    - `Llegada Cliente`
    - `Fin Entrega`
    - `Cancelado`
    ## Respuestas
    ```json title="Éxito" linenums="1" hl_lines="2"
    {
        "message": "trip updated correctly",
        "id": 12345
    }
    ``` 
    ```json title="Error" linenums="1" hl_lines="2"
    {
        "detail": "Mensaje específico del error",
        "status_code": 400/401/404/500
    }
    ``` 
=== "Flujo Diagrama"
    ```mermaid
    graph TD
        A[Cliente] --> B{Validar JWT}
        B -->|Invalid| C[Error 401]
        B -->|Valid| D{Tiene HoraConfirmacion?}
        
        D -->|Si| E[Buscar Viaje Existente]
        D -->|No| F[Crear Nuevo Viaje]
        
        E --> G{Viaje Encontrado?}
        G -->|No| H[Error 404]
        G -->|Si| I[Actualizar Campos]
        
        I --> J{Tiene HoraFinEntrega?}
        J -->|Si| K[Marcar Scheduled como Terminado]
        
        I --> L{Esta Cancelado?}
        L -->|Si| M[Actualizar Estados]
        
        F --> N[Crear ActualTrip]
        
        K --> O[WebPubSub]
        M --> O
        N --> O
        
        O --> P[Respuesta Success]
    ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Client
        participant API
        participant DB
        participant WebPubSub
        
        Client->>API: POST /post_trip
        API->>API: Validar JWT
        
        alt HoraConfirmacion existe
            API->>DB: Buscar último viaje
            DB-->>API: Viaje encontrado
            API->>DB: Actualizar campos
            
            alt HoraFinEntrega presente
                API->>DB: Actualizar ScheduledTrip
            end
            
            alt Viaje cancelado
                API->>DB: Actualizar estados
            end
            
        else HoraConfirmacion no existe
            API->>DB: Crear nuevo ActualTrip
        end
        
        API->>WebPubSub: Notificar cambios
        API-->>Client: Respuesta exitosa
    ```