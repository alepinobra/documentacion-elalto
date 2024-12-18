# API Viajes Programados - Documentación

## Descripción General
Sistema de gestión de viajes programados que permite consultar y administrar viajes programados, con diferentes niveles de acceso para administradores y conductores.

??? info "Notas Importantes"
    - Sistema de gestión de viajes programados
    - Diferentes niveles de acceso (admin/conductor)
    - Modelos principales:
        ```python title="modelos"
        class GetScheduleRequest(BaseModel):
            RutConductor: str
            
        class CancelScheduledRequest(BaseModel):
            Id: int
            Comentario: Optional[str] = None
        ```

??? warning "Consideraciones de Seguridad"
    - Validar permisos de usuario vs admin
    - Validar existencia de viajes
    - Manejar estados de cancelación
    - Proteger información sensible
    - Validar tokens JWT según rol

## Endpoints Viajes Programados

=== "Endpoints Administrativos"
    ??? success "Listar Todos los Viajes Programados (Admin)"
        ```python title="POST /api/logipath/scheduled_trips"
        @router.post(f"{APP_PATH}/scheduled_trips")
        async def scheduled_trips(sec: Annotated[str, Depends(security_scheme)]):
            """
            Retorna todos los viajes programados para visualización administrativa.
            Requiere permisos de administrador.
            
            Args:
                sec (str): Token JWT de administrador
                
            Returns:
                dict: {
                    "results": [
                        {
                            "Id": int,
                            "FechaProgramadaUtc": datetime,
                            "PlantaOrigen": str,
                            "Cliente": str,
                            "Programador": str,
                            "RutConductor": str,
                            "Patente": str,
                            "Terminado": bool,
                            "Cancelado": bool,
                            "Comentario": str,
                            "CodigoTransportista": str,
                            "UserName": str
                        }
                    ]
                }
                
            Raises:
                HTTPException: 
                    - 404: Si no hay viajes programados
                    - 401: Si el token es inválido
                    - 403: Si no tiene permisos de admin
            """
        ```

    ??? success "Cancelar Viaje Programado"
        ```python title="POST /api/logipath/cancel_scheduled"
        @router.post(f"{APP_PATH}/cancel_scheduled")
        async def cancel_scheduled(
            sec: Annotated[str, Depends(security_scheme)], 
            req: CancelScheduledRequest
        ):
            """
            Cancela un viaje programado específico.
            
            Args:
                sec (str): Token JWT de autenticación
                req (CancelScheduledRequest): ID del viaje y comentario opcional
                
            Returns:
                dict: {"message": "scheduled_trip Id:{id} cancelled successfully"}
            """
        ```

=== "Endpoints Conductores"
    ??? success "Consultar Viajes Asignados (Conductor)"
        ```python title="POST /api/logipath/get_schedule"
        @router.post(f"{APP_PATH}/get_schedule")
        async def get_schedule(
            sec: Annotated[str, Depends(security_scheme)], 
            req: GetScheduleRequest
        ):
            """
            Retorna los viajes programados para un conductor específico.
            
            Args:
                sec (str): Token JWT del conductor
                req (GetScheduleRequest): Rut del conductor
                
            Returns:
                dict: {"results": [Lista de viajes programados]}
            """
        ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Admin
        participant Conductor
        participant API
        participant DB
        
        alt Vista Administrativa
            Admin->>API: POST /scheduled_trips
            API->>API: Validar JWT Admin
            API->>DB: Query Todos los Viajes
            DB->>DB: Join con Users
            DB-->>API: Viajes + Info Usuario
            API-->>Admin: Lista Completa
        
        else Vista Conductor
            Conductor->>API: POST /get_schedule
            API->>API: Validar JWT
            API->>DB: Query Viajes del Conductor
            DB-->>API: Viajes Filtrados
            API-->>Conductor: Viajes Asignados
        
        else Cancelación
            Admin->>API: POST /cancel_scheduled
            API->>API: Validar JWT
            API->>DB: Actualizar Estado
            DB-->>API: Confirmación
            API-->>Admin: Success
        end
    ```

=== "Ejemplos de Respuesta"
    ??? example "Lista Administrativa"
        ```json
        {
            "results": [
                {
                    "Id": 1,
                    "FechaProgramadaUtc": "2024-03-20T10:00:00",
                    "PlantaOrigen": "Planta A",
                    "Cliente": "Cliente X",
                    "Programador": "Admin1",
                    "RutConductor": "12345678-9",
                    "Patente": "ABCD12",
                    "Terminado": false,
                    "Cancelado": false,
                    "Comentario": null,
                    "CodigoTransportista": "TRANS1",
                    "UserName": "Juan Pérez"
                }
            ]
        }
        ```
    
    ??? example "Lista Conductor"
        ```json
        {
            "results": [
                {
                    "Id": 1,
                    "FechaProgramadaUtc": "2024-03-20T10:00:00",
                    "PlantaOrigen": "Planta A",
                    "Cliente": "Cliente X",
                    "Patente": "ABCD12"
                }
            ]
        }
        ```

=== "Diferencias por Rol"
    | Característica | Vista Admin | Vista Conductor |
    |----------------|-------------|-----------------|
    | Alcance | Todos los viajes | Solo viajes propios |
    | Período | 2 días atrás | Hoy + 2 días |
    | Info Usuario | Incluida | No incluida |
    | Permisos | Completos | Solo lectura |
    | Cancelación | Permitida | No permitida |

??? note "Detalles Técnicos"
    - Vista Admin:
        - Join con tabla Users
        - Incluye información completa
        - Muestra todos los conductores
    - Vista Conductor:
        - Filtrada por RutConductor
        - Información limitada
        - Rango de fechas específico
    - Cancelación:
        - Actualiza estados
        - Registra comentarios
        - Mantiene historial

## Viajes Realizados

=== "Endpoint Viajes Realizados"
    ??? success "Consultar Viajes Recientes (Admin)"
        ```python title="POST /api/logipath/recent_trips_mes"
        @router.post(f"{APP_PATH}/recent_trips_mes")
        async def recent_trips_mes(sec: Annotated[str, Depends(security_scheme)]):
            """
            Retorna los viajes realizados en las últimas 5 semanas con estadísticas
            y detalles de productos desde SAP.
            Requiere permisos de administrador.
            
            Args:
                sec (str): Token JWT de administrador
                
            Returns:
                dict: {
                    "results": [Lista de viajes con detalles],
                    "cantidad_x_dia_semana": [Estadísticas por día]
                }
                
            Raises:
                HTTPException: 
                    - 404: Si no hay viajes recientes
                    - 401: Si el token es inválido
                    - 500: Error interno del servidor
            """
        ```

??? info "Estructura de Datos"
    ```python title="Modelo de Respuesta"
    {
        "results": [
            {
                "Id": int,
                "IdProgramado": int,
                "PlantaOrigen": str,
                "Cliente": str,
                "RutConductor": str,
                "Patente": str,
                "HoraInicioJornada": datetime,
                "HoraFinEntrega": datetime,
                "Folio": str,
                "Cancelado": bool,
                "RutTransportista": str,
                "NombreSapTransportista": str,
                "Articulo": str,
                "ItemName": str,
                "ForeignName": str,
                # ... más campos
            }
        ],
        "cantidad_x_dia_semana": [
            {
                "DiaSemana": str,
                "Cantidad": int
            }
        ]
    }
    ```

??? warning "Consideraciones Importantes"
    - Integración con SAP para detalles de productos
    - Conversión de zonas horarias (UTC a Chile)
    - Manejo de coordenadas geográficas
    - Gestión de estados de viaje
    - Tracking de eventos del viaje

??? tip "Características Principales"
    - Historial de 5 semanas
    - Estadísticas por día de la semana
    - Información detallada de productos
    - Tracking completo del viaje
    - Geolocalización en puntos clave

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Admin
        participant API
        participant DB
        participant SAP
        
        Admin->>API: POST /recent_trips_mes
        API->>API: Validar JWT Admin
        
        API->>DB: Query Viajes Recientes
        DB->>DB: Join Tables
        DB-->>API: Datos Base
        
        API->>API: Procesar Estadísticas
        
        opt Información SAP
            API->>SAP: Login SAP
            SAP-->>API: Token
            API->>SAP: Query Items
            SAP-->>API: Datos Productos
            API->>API: Enriquecer Datos
        end
        
        API-->>Admin: Resultados + Estadísticas
    ```

=== "Ejemplos de Respuesta"
    ??? example "Respuesta Exitosa"
        ```json
        {
            "results": [
                {
                    "Id": 1,
                    "PlantaOrigen": "Planta Central",
                    "Cliente": "Cliente A",
                    "HoraInicioJornada": "2024-03-20T08:00:00-03:00",
                    "Folio": "F001",
                    "Articulo": "ART001",
                    "ItemName": "Arena Fina",
                    "Neto": 28500
                }
            ],
            "cantidad_x_dia_semana": [
                {
                    "DiaSemana": "Monday",
                    "Cantidad": 12
                },
                {
                    "DiaSemana": "Tuesday",
                    "Cantidad": 15
                }
            ]
        }
        ```
    
    ??? example "Error Sin Datos"
        ```json
        {
            "detail": "No hay viajes recientes."
        }
        ```

=== "Campos Relevantes"
    | Campo | Descripción | Tipo |
    |-------|-------------|------|
    | HoraInicioJornada | Inicio del viaje | datetime |
    | HoraFinEntrega | Fin del viaje | datetime |
    | Folio | Número de guía | string |
    | Articulo | Código producto | string |
    | ItemName | Nombre producto | string |
    | Neto | Peso neto | number |
    | Cancelado | Estado cancelación | boolean |

??? note "Integración SAP"
    - Conexión automática a SAP
    - Obtención de detalles de productos
    - Manejo de errores de conexión
    - Caché de datos frecuentes
    - Timeout configurable

??? tip "Mejores Prácticas"
    - Monitorear tiempos de respuesta
    - Implementar caché cuando sea posible
    - Logging de errores SAP
    - Validar datos geográficos
    - Mantener índices optimizados