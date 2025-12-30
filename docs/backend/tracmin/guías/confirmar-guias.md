# API Confirmación de Guías - Documentación

## Descripción General
Endpoints para la confirmación y rectificación de guías de despacho validando que los pesos registrados en SAP coincidan con los pesos reales de las básculas (tickets).

??? info "Notas Importantes"
    - El sistema valida diferencias de peso entre guía SAP y ticket de báscula
    - Umbral de tolerancia: 500 kg de diferencia
    - Maneja tres casos: Confirmación Normal, Devolución y Nueva Guía
    - Requiere autenticación especial (header Authorization) para confirmar
    - Las guías con tickets objetados no pueden ser confirmadas
    - Se mantiene caché de sesión SAP por ~28 minutos
    - Paginación automática para listar guías pendientes
    - Las confirmaciones son irreversibles una vez completadas

??? warning "Consideraciones Importantes"
    - Las diferencias > 500 kg requieren confirmación manual forzada
    - El proceso es transaccional: si falla, se revierten cambios
    - Las rectificaciones modifican directamente el peso en SAP
    - Las devoluciones crean un documento Return en SAP
    - Las nuevas guías crean un documento complementario
    - Se realiza join entre guías SAP y viajes locales por Folio

??? info "Seguridad"
    - Autenticación JWT para listar guías pendientes
    - Autenticación por header especial para confirmar guías
    - Todas las comunicaciones con SAP son encriptadas
    - Se validan todos los inputs para prevenir inyecciones
    - Log de auditoría de todas las confirmaciones

## Endpoints

## Endpoints

### 1. Listar Guías Pendientes
`POST /api/logipath/unconfirmed_guides`

=== "Flujo Explicado"
    ### 1. Autenticación
    - `Validación de token JWT`
    - `Verificación de permisos`

    ### 2. Consulta en SAP
    - `Login en SAP con caché (TTL 28 minutos)`
    - `Filtro: U_SEI_CONF = 'N' y fecha últimas 2 semanas`
    - `Paginación automática para obtener todas las guías`

    ### 3. Consulta en Base de Datos Local
    - `Query a tabla ActualTrip con Folio != NULL`
    - `Filtro: fecha últimas 2 semanas`

    ### 4. Join de Datos
    - `Join entre guías SAP y viajes locales por Folio`
    - `Generación de diccionarios para optimizar búsqueda`
    - `Combinación de información SAP y local`

    ```json title="Request Body" linenums="1"
    {
        // No requiere parámetros
    }
    ```

    ```json title="Response" linenums="1" hl_lines="2-6"
    {
        "message": "guides found",
        "results": {
            "total_sap_guides": 150,
            "total_local_trips": 145,
            "total_joined": 140,
            "total_matched_folios": 138,
            "matched_folios": [12345, 12346],
            "joined_results": [
                {
                    "sap_guide": {
                        "FolioNumber": 12345,
                        "DocEntry": 123456,
                        "CardCode": "C12345678-9",
                        "U_SEI_CONF": "N",
                        "Confirmed": "tYES",
                        "DocumentLines": [...]
                    },
                    "local_trip": {
                        "Id": "uuid",
                        "Folio": 12345,
                        "Patente": "ABC123",
                        "Cliente": "Cliente Ejemplo",
                        "PesoConfirmadoManual": 25000
                    }
                }
            ]
        }
    }
    ```

=== "Estados SAP"
    ## Campos de Control

    | Campo | Valores | Descripción |
    |-------|---------|-------------|
    | `U_SEI_CONF` | "N", "Y" | Indica si está confirmada |
    | `Confirmed` | "tNO", "tYES" | Estado de confirmación SAP |
    | `U_SEI_Procesado` | "N", "P" | Indica si fue procesada |

    ## Combinaciones de Estados
    
    **Pendiente de Confirmación:**
    ```
    U_SEI_CONF = "N"
    Confirmed = "tYES"
    U_SEI_Procesado = "N"
    ```

    **Confirmada:**
    ```
    U_SEI_CONF = "Y"
    Confirmed = "tYES"
    U_SEI_Procesado = "P"
    ```

=== "Flujo Diagrama"
    ```mermaid
    graph TD
        A[Cliente] --> B{Validar JWT}
        B -->|Invalid| C[Error 401]
        B -->|Valid| D[Login SAP con Caché]
        
        D --> E[Consultar Guías SAP]
        E --> F[U_SEI_CONF = N últimas 2 semanas]
        F --> G[Paginación Automática]
        
        G --> H[Consultar Viajes Locales]
        H --> I[ActualTrip con Folio]
        
        I --> J[Join por Folio]
        J --> K[Generar Resultado Combinado]
        K --> L[Retornar JSON]
    ```

### 2. Confirmar o Rectificar Guía
`POST /api/logipath/confirm_guide`

=== "Flujo Explicado"
    ### 1. Validación de Seguridad
    - `Validación de header Authorization`
    - `Verificación de key especial: confirmar_guia`

    ### 2. Búsqueda en SAP
    - `Login en SAP`
    - `Búsqueda por FolioNumber`
    - `Obtención de DocEntry y DocumentLines`

    ### 3. Cálculo de Diferencia
    - `Quantity (SAP) - PesoTicket (Real)`
    - `Conversión de unidades si es necesario`
    - `Validación de umbral (500 kg)`

    ### 4. Procesamiento Según Caso
    - **Rectificación Manual:** Modificar peso directamente
    - **Devolución:** Crear documento Return
    - **Nueva Guía:** Crear guía complementaria
    - **Normal:** Actualizar estado de confirmación

    ```json title="Request Body" linenums="1" hl_lines="2-4"
    {
        "Folio": 12345,
        "PesoTicket": 25000,
        "PesoGuia": 25400,
        "ForzarConfirmacion": false,
        "Rectificacion": false,
        "PesoGuiaModificado": null
    }
    ```

    ```python title="Esquema Pydantic" linenums="1"
    class reqConfirmar(BaseModel):
        Folio: int                          # Folio de la guía
        PesoTicket: float                   # Peso real báscula (kg)
        PesoGuia: Optional[float]           # Peso SAP (kg)
        ForzarConfirmacion: bool = False    # Forzar confirmación manual
        Rectificacion: bool = False         # Indica rectificación
        PesoGuiaModificado: Optional[float] # Nuevo peso rectificación
    ```

=== "Casos de Confirmación"
    ## A. Rectificación Manual
    **Cuando:** Usuario modifica peso manualmente
    
    ```python title="Lógica" linenums="1"
    if req.Rectificacion and req.PesoGuiaModificado:
        json_patch = {
            "U_SEI_CONF": "Y",
            "Confirmed": "tYES",
            "U_SEI_Procesado": "P",
            "U_SEI_PESO": nuevo_peso,
            "Comments": f"Rectificación. Peso modificado a {nuevo_peso} kg."
        }
        # PATCH a /b1s/v1/DeliveryNotes({docentry})
    ```

    ```json title="Respuesta Éxito" linenums="1"
    {
        "message": "Guía rectificada correctamente",
        "docentry_confirm": 123456,
        "nuevo_peso": 25500,
        "rectificacion": true
    }
    ```

    ## B. Devolución (Peso Guía > Peso Ticket)
    **Cuando:** diferencia > 0 (hay exceso en la guía)
    
    ```python title="Lógica" linenums="1"
    if diferencia > 0:
        # 1. Crear documento Return
        json_return = {
            "CardCode": CardCode,
            "U_SEI_DocGuiaDev": req.Folio,
            "DocumentLines": [{
                "BaseType": 15,
                "ItemCode": ItemCode,
                "Quantity": (Quantity - req.PesoTicket)/1000,
                "BaseEntry": docentry,
                "BaseLine": BaseLine
            }]
        }
        # POST a /b1s/v1/Returns
        
        # 2. Actualizar guía original
        json_patch = {
            "U_SEI_CONF": "Y",
            "Confirmed": "tYES",
            "U_SEI_Procesado": "P",
            "U_SEI_DocGuiaDev": docentry_return,
            "U_SEI_PESO": peso_confirmado/1000
        }
        # PATCH a /b1s/v1/DeliveryNotes({docentry})
    ```

    ```json title="Respuesta Éxito" linenums="1"
    {
        "message": "Guía confirmada y devolución generada",
        "docentry_return": 123457,
        "docentry_confirm": 123456
    }
    ```

    ## C. Nueva Guía (Peso Ticket > Peso Guía)
    **Cuando:** diferencia < 0 (falta peso en la guía)
    
    ```python title="Lógica" linenums="1"
    if diferencia < 0:
        # Crear guía complementaria
        json_4_sap = {
            "DocType": "dDocument_Items",
            "CardCode": CardCode,
            "FolioNumber": req.Folio,
            "U_SEI_CONF": "Y",
            "Confirmed": "tYES",
            "U_SEI_Procesado": "P",
            "DocumentLines": [{
                "ItemCode": ItemCode,
                "Quantity": (req.PesoTicket - Quantity)/1000
            }]
        }
        # POST a /b1s/v1/DeliveryNotes
        
        # Actualizar guía original
        json_patch = {
            "U_SEI_CONF": "Y",
            "Confirmed": "tYES",
            "U_SEI_Procesado": "P",
            "U_SEI_DocGuiaEnt": docentry_new
        }
    ```

    ```json title="Respuesta Éxito" linenums="1"
    {
        "message": "Guía confirmada y nueva guía generada",
        "docentry_new": 123457,
        "docentry_confirm": 123456
    }
    ```

    ## D. Confirmación Normal
    **Cuando:** |diferencia| <= 500 kg
    
    ```python title="Lógica" linenums="1"
    if abs(diferencia) <= 500:
        json_patch = {
            "U_SEI_CONF": "Y",
            "Confirmed": "tYES",
            "U_SEI_Procesado": "P",
            "U_SEI_PESO": peso_ticket/1000
        }
        # PATCH a /b1s/v1/DeliveryNotes({docentry})
    ```

    ```json title="Respuesta Éxito" linenums="1"
    {
        "message": "Guía confirmada correctamente",
        "docentry_confirm": 123456
    }
    ```

    ## Requiere Confirmación Manual
    **Cuando:** |diferencia| > 500 kg y ForzarConfirmacion = false
    
    ```json title="Respuesta" linenums="1"
    {
        "message": "El peso no se puede confirmar automáticamente, confirme manualmente",
        "diferencia": 750,
        "response_invoices": {...}
    }
    ```

=== "Respuestas y Errores"
    ## Respuestas de Error

    ```json title="Error: Guía No Encontrada" linenums="1"
    {
        "message": "no se encontró la guía en SAP",
        "status_code": 500
    }
    ```

    ```json title="Error: Login SAP" linenums="1"
    {
        "message": "no se pudo iniciar sesión en sap",
        "status_code": 500
    }
    ```

    ```json title="Error: Autenticación" linenums="1"
    {
        "detail": "Unauthorized",
        "status_code": 401
    }
    ```

=== "Flujo Completo"
    ```mermaid
    graph TD
        A[Frontend: Confirmar Guía] --> B{Validar Authorization}
        B -->|Invalid| C[Error 401]
        B -->|Valid| D[Login SAP]
        
        D --> E[Buscar Guía por Folio]
        E -->|No existe| F[Error: Guía no encontrada]
        E -->|Existe| G[Calcular Diferencia]
        
        G --> H{Tipo de Operación}
        
        H -->|Rectificación| I[Modificar Peso SAP]
        H -->|Diferencia > 0| J[Crear Devolución]
        H -->|Diferencia < 0| K[Crear Nueva Guía]
        H -->|Normal| L[Actualizar Estado]
        H -->|Diferencia > 500kg y !Forzar| M[Requiere Confirmación Manual]
        
        I --> N[PATCH DeliveryNotes]
        J --> O[POST Returns + PATCH Original]
        K --> P[POST DeliveryNotes + PATCH Original]
        L --> N
        
        N --> Q[Respuesta Éxito]
        O --> Q
        P --> Q
        M --> R[Respuesta 409: Confirmar Manual]
    ```

## Modelos de Base de Datos

=== "ActualTrip"
    ```python title="Campos Relevantes" linenums="1"
    class ActualTrip(Base):
        __tablename__ = "ActualTrip"
        
        Id: UUID                      # PK
        Folio: int                    # Folio de la guía
        HoraConfirmacionFolio: datetime
        PesoConfirmadoManual: float   # Peso del ticket
        IdTicket: str                 # ID del ticket de báscula
        Cancelado: bool
        Romana: bool                  # Indica si tiene romana
        Cliente: str
        Patente: str
        RutConductor: str
    ```

=== "GuiaStatus"
    ```python title="Control de Estados" linenums="1"
    class GuiaStatus(Base):
        __tablename__ = "GuiaStatus"
        
        IdViaje: UUID                 # FK -> ActualTrip
        DocEntry: int                 # DocEntry de SAP
        Folio: int                    # Folio de la guía
        FebosId: str                  # ID de Febos
        Status: str                   # Estado actual
        HoraModificacion: datetime
    ```

## Sistema de Caché

=== "Implementación"
    ```python title="TTL Cache" linenums="1"
    class TTLCache:
        def __init__(self, ttl_seconds):
            self.ttl_seconds = ttl_seconds
            self.cache = {}
            self.timestamps = {}

        def get(self, key):
            if key in self.cache:
                if time.time() - self.timestamps[key] < self.ttl_seconds:
                    return self.cache[key]
                else:
                    del self.cache[key]
                    del self.timestamps[key]
            return None

        def set(self, key, value):
            self.cache[key] = value
            self.timestamps[key] = time.time()

    # Caché de sesión SAP: ~28 minutos
    sap_session_cache = TTLCache(ttl_seconds=1700)
    ```

=== "Ventajas"
    - ✅ Reduce llamadas de login a SAP
    - ✅ Mejora tiempo de respuesta
    - ✅ Evita sobrecarga del servidor SAP
    - ✅ TTL configurable por tipo de caché

## Configuración

=== "Variables de Entorno"
    ```python title="config.py" linenums="1"
    LINK_SAP = "https://elalto.cloudseidor.com:50000"
    USER_DESARROLLO = "usuario_sap"
    PASS_DESARROLLO = "password_sap"
    COMPANYDB_SAP_MIGRIN = "SBOMIGRIN"
    CONFIRMAR_GUIA_SAP = "confirmar_guia"  # Key autenticación
    ```

## Mejores Prácticas

??? tip "Validación de Pesos"
    - Siempre verificar que los pesos sean positivos
    - Convertir toneladas a kilogramos cuando sea necesario  
    - Aplicar umbral de tolerancia (500 kg)
    - Validar formato numérico antes de procesar

??? tip "Manejo de Transacciones"
    - Usar sesiones de BD con context managers
    - Hacer commit solo después de confirmar en SAP
    - Implementar rollback en caso de error
    - Mantener atomicidad en operaciones críticas

??? tip "Logs y Auditoría"
    - Registrar todas las confirmaciones exitosas
    - Loguear errores con contexto completo
    - Mantener historial de rectificaciones
    - Timestamp en todas las operaciones

??? tip "Seguridad"
    - Usar autenticación por header para endpoints internos
    - Validar JWT para endpoints públicos
    - No exponer credenciales de SAP
    - Sanitizar todos los inputs

## Troubleshooting

??? question "Sesión SAP Expirada"
    **Síntoma:** Error de autenticación en SAP
    
    **Solución:** El caché de sesiones se regenera automáticamente después de 28 minutos.
    
    **Verificar:**
    ```python
    # El sistema maneja automáticamente la reconexión
    session = get_sap_session()
    if not session:
        # Se intenta crear nueva sesión
    ```

??? question "Guía No Aparece en Listado"
    **Síntoma:** La guía no se muestra en la lista de pendientes
    
    **Verificar:**
    - La guía tenga `U_SEI_CONF = "N"`
    - La fecha sea dentro de las últimas 2 semanas
    - El viaje tenga Folio asignado
    - La guía no esté cancelada

??? question "Error al Crear Devolución"
    **Síntoma:** Falla al crear documento Return
    
    **Validar:**
    - La guía original exista en SAP
    - Los campos `BaseEntry` y `BaseLine` sean correctos
    - El cliente permita devoluciones
    - Permisos en SAP para crear Returns

??? question "Diferencia de Peso Incorrecta"
    **Síntoma:** El cálculo de diferencia no es correcto
    
    **Verificar:**
    - Unidades de medida (kg vs toneladas)
    - Conversión: `Quantity < 1000 ? Quantity * 1000 : Quantity`
    - Que ambos valores sean numéricos válidos

## Referencias

- [Documentación SAP Service Layer](https://help.sap.com/doc/0d2533ad95ba4ad7a702e83570a21c32/10.0/en-US/SAPLMS_SL_API_Reference.pdf)
- [Crear Guías](./crear-guias.md)
- [Cancelar Guías](./cancel-guias.md)
