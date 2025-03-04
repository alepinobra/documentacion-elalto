# Guías Combustible - Documentación

## Flujo Proceso Consumos de Combustible
Proceso que automatiza la generación de guías de despacho para consumos de combustible, integrando datos desde Neotac con SAP y Febos.

??? note "Visión de Negocio"
    El proceso automatiza la generación de documentos tributarios para consumos de combustible siguiendo estos pasos clave:
    ??? info "1. Identificación de Consumos"
        Busca en la base de datos Neotac los consumos de combustible que no tienen guía asociada.
    ??? info "2. Validación de Datos"
        Verifica que el RUT del conductor y la patente del vehículo sean válidos y estén registrados.
    ??? info "3. Determinación de Precios"
        - Consulta precios actuales de combustible en API externa (bencinaenlinea.cl)
        - Obtiene impuestos específicos desde el SII mediante web scraping
        - Calcula precio final según el tipo de acuerdo comercial:
            - TipoAcuerdo 0: Precio copec menos impuesto específico, sin IVA, más margen
            - TipoAcuerdo 1: Precio de costo más margen
    ??? info "4. Generación de Documentos"
        - Crea guía de despacho en SAP con los datos del consumo
        - Genera XML para documento tributario electrónico
        - Envía a Febos para foliado
    ??? info "5. Actualización de Estado"
        - Actualiza la guía en SAP con el folio de Febos
        - Registra el estado del proceso en NeotacGuiasStatus
        - Obtiene link al documento para consulta posterior


??? info "Integraciones Clave"
    - SAP: Creación y actualización de guías de despacho
    - Febos: Timbrado electrónico y foliado de documentos tributarios
    - SII: Obtención de impuestos específicos a combustibles
    - API de Precios: Consulta de precios actuales de combustible

??? tip "Bases de Datos"
    | Tabla | Descripción | Datos Relevantes |
    |-------|-------------|------------------|
    | ConsumosNeotac | Registra las transacciones de combustible desde Neotac | Voucher, fecha, volumen, patente, RUT conductor, producto |
    | NeotacGuiasStatus | Almacena el estado de procesamiento de cada guía | Voucher, DocEntry, Folio, FebosId, Status, errores |
    | VehiculosNeoTac | Contiene información de los vehículos autorizados | Patente, Empresa, datos técnicos |
    | ClientesCombustible | Almacena información de clientes y sus acuerdos comerciales | Empresa, TipoAcuerdo, Margen|

## Ejemplos de Documentos

### Guía de Despacho de Combustible

Puedes descargar un ejemplo de guía de despacho de combustible generada por el sistema:

[:fontawesome-solid-file-pdf: Descargar ejemplo de Guía de Despacho (PDF)](https://drive.google.com/file/d/1fxUUNVmiT6e20YrFrOWtwx41wCL1DONw/view?usp=sharing){ .md-button }

!!! info "Nota sobre el documento"
    Este documento es un ejemplo de una guía de despacho generada por el sistema. Contiene todos los campos relevantes como información del cliente, conductor, vehículo, producto, cantidades y valores.

## Diagramas de Flujo

=== "Flujo"
    ```mermaid
    flowchart TD
    A["Inicio"] --> B["Obtener consumos sin procesar"]
    B --> C{"¿Hay consumos?"}
    C -- No --> D["Retornar: No hay consumos nuevos"]
    C -- Sí --> E["Procesar cada consumo"]
    E --> F["Validar datos del consumo"]
    F --> G{"¿Datos válidos?"}
    G -- No --> H["Registrar error y continuar"]
    H --> V["Continuar con siguiente consumo"]
    G -- Sí --> I["Login en SAP"]
    I --> J["Obtener precio combustible"]
    J --> K["Obtener datos impuestos SII"]
    K --> L["Crear guía en SAP"]
    L --> M["Registrar guía en base de datos"]
    M --> N["Obtener detalles adicionales"]
    N --> O["Generar XML para Febos"]
    O --> P["Enviar documento a Febos"]
    P --> Q{"¿Éxito en Febos?"}
    Q -- No --> R["Cancelar guía en SAP y registrar error"]
    R --> V
    Q -- Sí --> S["Actualizar guía en SAP con folio"]
    S --> T["Obtener link de documento"]
    T --> U["Actualizar estado final"]
    U --> V
    V --> W["Retornar resumen de procesamiento"]
    ```

=== "Diagrama de Secuencia"
    ```mermaid
    sequenceDiagram
    participant API
    participant DB
    participant SAP
    participant SII
    participant PrecioAPI
    participant Febos
    
    Note over API: Inicio del proceso
    
    API->>DB: Obtener consumos sin procesar
    DB-->>API: Lista de consumos
    
    alt No hay consumos
        API-->>API: Retornar "No hay consumos nuevos"
    else Hay consumos
        loop Para cada consumo
            API->>API: Validar datos del consumo (RUT, patente)
            
            alt Datos inválidos
                API->>DB: Registrar error
                Note over API: Continuar con siguiente consumo
            else Datos válidos
                API->>SAP: Login en SAP
                SAP-->>API: Sesión autenticada
                
                API->>PrecioAPI: Obtener precio combustible
                PrecioAPI-->>API: Precio actual
                
                API->>SII: Obtener datos impuestos SII
                SII-->>API: Componentes IEF, IEV, IER
                
                API->>SAP: Crear guía en SAP
                SAP-->>API: DocEntry
                
                API->>DB: Registrar guía en base de datos
                
                API->>SAP: Obtener detalles adicionales
                SAP-->>API: Datos cliente y dirección
                
                API->>API: Generar XML para Febos
                
                API->>Febos: Enviar documento a Febos
                
                alt Error en Febos
                    Febos-->>API: Error
                    API->>SAP: Cancelar guía en SAP
                    API->>DB: Registrar error
                    Note over API: Continuar con siguiente consumo
                else Éxito en Febos
                    Febos-->>API: Folio y FebosId
                    
                    API->>SAP: Actualizar guía en SAP con folio
                    
                    API->>Febos: Obtener link de documento
                    Febos-->>API: URL documento
                    
                    API->>DB: Actualizar estado final
                    Note over API: Continuar con siguiente consumo
                end
            end
        end
        API-->>API: Retornar resumen de procesamiento
    end
    ```

=== "Ejemplos de Respuesta"
    ??? example "Guía generada correctamente"
        ```json
        {
          "message": "Se procesaron 3 consumos correctamente y fallaron 1 consumos",
          "exitosos": [
            {
              "voucher": "12345",
              "folio": "987654",
              "link": "https://febos.cl/documento/987654",
              "docEntry": 12345,
              "monto_bruto": 50000,
              "monto_neto": 42016.81,
              "monto_iev": 1.25,
              "monto_ief": 3.75
            }
          ],
          "fallidos": [
            {
              "voucher": "67890",
              "error": "RUT inválido: 12345678-9",
              "patente": "ABCD12"
            }
          ]
        }
        ```
    
    ??? example "Error en la creación de la guía"
        ```json
        {
            "message": "Error al procesar los consumos",
            "error": str(e)
        }
        ```
=== "Códigos de Estado"
    | Código | Descripción |
    |--------|-------------|
    | 200 | Operación exitosa |
    | 401 | No autorizado |
    | 404 | Documento no encontrado |
    | 500 | Error interno |
