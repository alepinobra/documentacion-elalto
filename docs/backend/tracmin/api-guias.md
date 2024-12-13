# API SAP Guides - Documentación

## Descripción General
Endpoint para la creación y gestión de guías de despacho integrando SAP, Febos y base de datos interna.

## Endpoint Para Crear Guías
`POST /api/logipath/sap_guides`

=== "Flujo Explicado"

    ### 1. Verificación Inicial
    - `Verifica si ya existe una guía para el viaje`
    - `Si existe y está completa, retorna la información`
    - `Si existe pero está incompleta, intenta completar el proceso`

    ### 2. Creación en SAP
    - `Login en SAP`
    - `Creación de la guía con datos básicos`
    - `Registro en GuiaStatus como "guia_creada_sap"`
    - `Obtención de datos adicionales del cliente y detalles`

    ### 3. Creación en Febos
    - `Generación del XML`
    - `Envío a Febos para foliar`
    - `Verificación de folio duplicado en SAP`
    - `Actualización de GuiaStatus según resultado`

    ### 4. Actualización en SAP
    - `Patch con folio y ID de Febos`
    - `Obtención del link de la imagen`
    - `Actualización de GuiaStatus`

    ### 5. Actualización Base de Datos
    - `Actualización de ActualTrip con folio y datos de ubicación`
    - `Actualización final de GuiaStatus`
    - `Envío de notificación WebPubSub`

=== "Estados de GuiaStatus"

    ## Estados de GuiaStatus
    - `guia_creada_sap`
    - `error_folio_repetido`
    - `error_verificacion_sap`
    - `error_patch_sap`
    - `error_obtener_link_febos`
    - `guia_creada_sin_link`
    - `guia_creada_correctamente`
    - `error_viaje_no_encontrado`
    - `error_actualizacion_viaje`
    - `error_general`
    - `proceso_completado`

    ## Manejo de Errores
    - `Errores de conexión SAP`
    - `Errores de Febos`
    - `Folios duplicados`
    - `Errores de base de datos`
    - `Errores de actualización`
    - `Errores generales del proceso`

    ## Respuestas

    ```json title="Éxito" linenums="1" hl_lines="2"
    {
        "message": "guía generada correctamente",
        "FolioNumber": "12345",
        "datalink": "https://..."
    }
    ```

    ```json title="Error" linenums="1" hl_lines="2"
    {
        "message": "Error específico",
        "error": "Detalle del error"
    }
    ```

    ```bash title="Estructura de la API" linenums="1"
    SAP_GUIDES/
    │
    ├─Verificación_Inicial/
    │ ├── Consulta_GuiaStatus
    │ └── Verifica_estado_existente
    │
    ├─SAP/
    │ ├── Login
    │ ├── Crear_Guía
    │ ├── Obtener_Datos_Cliente
    │ └── Patch_Folio
    │
    ├─Febos/
    │ ├── Generar_XML
    │ ├── Obtener_Folio
    │ └── Obtener_Link
    │
    ├─Base_de_Datos/
    │ ├── GuiaStatus
    │ │ ├── Registro_inicial
    │ │ ├── Actualizaciones_de_estado
    │ │ └── Registro_de_errores
    │ │
    │ └── ActualTrip
    │ └── Actualización_con_folio
    │
    └─Notificaciones/
    └── WebPubSub
    ```
=== "Flujo Diagrama"

    # Flujo API SAP Guides

    ```mermaid
    graph TD
        %% Main Endpoints
        A[Client] --> B{API Endpoints}
        B --> C[POST /sap_guides]
        B --> D[POST /cancel_guide]
        
        %% SAP Guides Flow
        C --> E{Verificar Estado Guía}
        E -->|Guía Existe| F[Retornar Guía Existente]
        E -->|No Existe| G[Crear Nueva Guía]
        
        %% Create Guide Flow
        G --> H[Login SAP]
        H --> I[Crear Guía en SAP]
        I --> J[Registrar en GuiaStatus]
        J --> K[Crear Guía en FEBOS]
        
        %% FEBOS Processing
        K -->|Success| L[Actualizar SAP con Folio]
        K -->|Error| M[Cancelar en SAP]
        
        %% Final Steps
        L --> N[Obtener Link FEBOS]
        N --> O[Actualizar ActualTrip]
        O --> P[Enviar WebPubSub]
        
        %% Cancel Guide Flow
        D --> Q[Verificar Guía]
        Q --> R[Login SAP]
        R --> S[Crear Return en SAP]
        S --> T[Actualizar GuiaStatus]
        
        %% Status Updates
        J --> U((DB Updates))
        L --> U
        O --> U
        T --> U
        
        %% Error Handling
        M --> V[Error Handler]
        V --> W[Log Error]
        W --> X[Update Status]
        
        %% Response Paths
        F --> Y[API Response]
        P --> Y
        T --> Y
        X --> Y
    ```

    ```mermaid
    sequenceDiagram
        autonumber
        participant Client
        participant API
        participant SAP
        participant FEBOS
        participant DB
        participant WebPubSub

        Client->>API: POST /sap_guides
        API->>DB: Verificar estado guía
        
        alt Guía existe
            DB-->>API: Retornar guía existente
            API-->>Client: Respuesta con guía
        else No existe
            API->>SAP: Login
            SAP-->>API: Session OK
            API->>SAP: Crear guía
            SAP-->>API: Guía creada
            
            API->>DB: Registrar GuiaStatus
            DB-->>API: Status registrado
            
            API->>FEBOS: Crear guía + XML
            
            alt Success FEBOS
                FEBOS-->>API: Folio asignado
                API->>SAP: Actualizar con folio
                API->>FEBOS: Obtener link
                FEBOS-->>API: Link documento
                API->>DB: Actualizar ActualTrip
                API->>WebPubSub: Notificar cambios
            else Error FEBOS
                API->>SAP: Cancelar guía
                API->>DB: Registrar error
            end
            
            API-->>Client: Respuesta final
        end
    ```
-----

## Endpoint Para Cancelar Guías
`POST /api/logipath/cancel_guide`

=== "Flujo Explicado"

=== "Estados de GuiaStatus"

=== "Flujo Diagrama"
