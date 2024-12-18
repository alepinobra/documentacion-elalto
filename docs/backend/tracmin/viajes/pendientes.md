# API Viajes Pendientes - Documentación

## Descripción General
Sistema de gestión de viajes pendientes que permite consultar y actualizar el estado de viajes que quedaron incompletos, permitiendo su finalización desde el panel administrativo.

??? info "Notas Importantes"
    - Sistema de gestión de viajes pendientes
    - Requiere autenticación JWT
    - Modelos principales:
        ```python title="modelos"
        class TripRequest(BaseModel):
            IdProgramado: int
            RutConductor: str
            HoraInicioJornada: datetime
            HoraFinEntrega: Optional[datetime]
            Cancelado: Optional[bool]
            Comentario: Optional[str]
            # ... otros campos opcionales
            
        class GetScheduleRequest(BaseModel):
            RutConductor: str
        ```
??? warning "Consideraciones Importantes"
    - Período de búsqueda: 30 días o desde 12-08-2024
    - Validación de estados incompletos
    - Actualización atómica de estados
    - Manejo de transacciones DB
    - Logging de cambios

??? tip "Mejores Prácticas"
    - Validar datos antes de actualizar
    - Mantener consistencia de estados
    - Documentar cambios realizados
    - Verificar permisos de usuario
    - Mantener trazabilidad

??? note "Estados de Viaje"
    - **Pendiente**: 
        - Con Folio
        - Sin peso confirmado
        - Sin pausas registradas
    - **Finalizado**:
        - HoraFinEntrega registrada
        - Viaje programado marcado como terminado
    - **Cancelado**:
        - Estado cancelado actualizado
        - Comentario registrado
        - Viaje programado marcado como terminado
        
## Endpoints Viajes Pendientes

=== "Endpoints"
    ??? success "Consultar Viajes Pendientes"
        ```python title="POST /api/logipath/get_pending_trip"
        @router.post(f"{APP_PATH}/get_pending_trip")
        async def get_pending_trip(
            sec: Annotated[str, Depends(security_scheme)], 
            req: GetScheduleRequest
        ):
            """
            Retorna los viajes pendientes de finalización para un conductor específico.
            
            Args:
                sec (str): Token JWT
                req (GetScheduleRequest): Rut del conductor
                
            Returns:
                dict: {
                    "results": {
                        Información detallada del viaje pendiente
                    }
                }
                
            Raises:
                HTTPException: 
                    - 404: Si no hay viajes pendientes
                    - 500: Error interno
            """
        ```
    
    ??? success "Actualizar Viaje Pendiente"
        ```python title="POST /api/logipath/post_trip_pending"
        @router.post(f"{APP_PATH}/post_trip_pending")
        async def post_trip(
            sec: Annotated[str, Depends(security_scheme)], 
            req: TripRequest
        ):
            """
            Actualiza el estado de un viaje pendiente.
            
            Args:
                sec (str): Token JWT
                req (TripRequest): Datos de actualización del viaje
                
            Returns:
                dict: {"message": "Trip updated correctly"}
                
            Raises:
                HTTPException: Si hay error en la actualización
            """
        ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Admin
        participant API
        participant DB
        
        alt Consultar Pendientes
            Admin->>API: POST /get_pending_trip
            API->>API: Validar JWT
            API->>DB: Query Viajes Pendientes
            
            alt Viaje Encontrado
                DB-->>API: Datos Viaje
                API-->>Admin: Información Viaje
            else Sin Pendientes
                API-->>Admin: Error 404
            end
        
        else Actualizar Pendiente
            Admin->>API: POST /post_trip_pending
            API->>API: Validar JWT
            API->>DB: Buscar Viaje
            
            alt Viaje Existe
                API->>DB: Actualizar Estados
                DB-->>API: Confirmación
                
                opt Finalización
                    API->>DB: Marcar Programado como Terminado
                end
                
                opt Cancelación
                    API->>DB: Marcar como Cancelado
                    API->>DB: Actualizar Comentario
                end
                
                API-->>Admin: Success
            else No Existe
                API-->>Admin: "Trip not found"
            end
        end
    ```

=== "Criterios de Búsqueda"
    | Campo | Descripción |
    |-------|-------------|
    | Folio | No nulo |
    | HoraConfirmacionFolio | >= start_date |
    | PesoConfirmadoManual | Nulo |
    | CantidadPausa | Nulo |

=== "Ejemplos de Respuesta"
    ??? example "Consulta Viaje Pendiente"
        ```json
        {
            "results": {
                "Id": 1,
                "IdProgramado": 100,
                "PlantaOrigen": "Planta A",
                "Cliente": "Cliente X",
                "RutConductor": "12345678-9",
                "Patente": "ABCD12",
                "HoraInicioJornada": "2024-03-20T08:00:00",
                "Folio": "F001",
                "Comentario": "PendingTrip"
            }
        }
        ```
    
    ??? example "Actualización Exitosa"
        ```json
        {
            "message": "Trip updated correctly"
        }
        ```

