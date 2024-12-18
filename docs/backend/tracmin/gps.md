# API Coordenadas - Documentación

## Descripción General
Sistema de tracking que integra múltiples fuentes de geolocalización: proveedores GPS externos, GPS de dispositivos móviles y visualización de coordenadas para seguimiento en tiempo real.

??? info "Modelos de Datos"
    ```python title="modelos"
    class transportistaCoordenadas(BaseModel):
        Transportista: Optional[str] = None
        Gps: Optional[str] = None
        Patente: Optional[str] = None
        Latitud: Optional[float] = None
        Longitud: Optional[float] = None
        Velocidad: Optional[float] = None
        HoraCoordenada: Optional[str] = None

    class CoordinateRequest(BaseModel):
        IdProgramado: int
        RutConductor: str
        HoraInicioJornada: datetime
        HoraCoordenada: datetime
        Longitud: float
        Latitud: float

    class GetCoordinatesRequest(BaseModel):
        IdViaje: int
    ```

## Endpoints de Coordenadas

=== "Webhook GPS Externos"
    ??? success "Recibir Coordenadas GPS"
        ```python title="POST /api/logipath/gps_coords"
        @router.post(f"{APP_PATH}/gps_coords")
        async def get_coords(
            req: Union[transportistaCoordenadas, List[transportistaCoordenadas]],
            request: Request
        ):
            """
            Webhook para recibir coordenadas de proveedores GPS externos.
            
            Args:
                req: Coordenadas del proveedor GPS
                request: Request con key de autorización
                
            Returns:
                dict: {
                    "status": 200,
                    "message": "Trip added correctly"
                }
                
            Validaciones:
                - Key de autorización válida
                - Formato de patente
                - Existencia del viaje
            """
        ```

=== "GPS Móvil"
    ??? success "Enviar Coordenadas App"
        ```python title="POST /api/logipath/la_ruta"
        @router.post(f"{APP_PATH}/la_ruta")
        async def post_coordinate(
            sec: Annotated[str, Depends(security_scheme)],
            req: CoordinateRequest
        ):
            """
            Registra coordenadas desde el GPS del dispositivo móvil.
            
            Args:
                sec: Token JWT
                req: Datos de coordenadas
                
            Returns:
                dict: {"message": "coordinate added successfully"}
                
            Validaciones:
                - Token JWT válido
                - Existencia del viaje
                - Formato de coordenadas
            """
        ```

=== "Consulta Coordenadas"
    ??? success "Obtener Coordenadas"
        ```python title="POST /api/logipath/get_coords"
        @router.post(f"{APP_PATH}/get_coords")
        async def get_coords(
            sec: Annotated[str, Depends(security_scheme)],
            req: Union[GetCoordinatesRequest, List[GetCoordinatesRequest]]
        ):
            """
            Obtiene coordenadas para visualización en mapa.
            
            Args:
                sec: Token JWT
                req: ID(s) de viaje(s)
                
            Returns:
                dict: {
                    "results": [Lista de coordenadas],
                    "not_found": [IDs no encontrados]
                }
            """
        ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant GPS Provider
        participant Mobile App
        participant API
        participant DB
        participant Map View
        
        par GPS Provider
            GPS Provider->>API: POST /gps_coords
            API->>API: Validar Key
            API->>DB: Guardar Coordenadas
            API-->>GPS Provider: Confirmación
            
        and Mobile App
            Mobile App->>API: POST /la_ruta
            API->>API: Validar JWT
            API->>DB: Guardar Coordenadas
            API-->>Mobile App: Confirmación
            
        and Map View
            Map View->>API: POST /get_coords
            API->>API: Validar JWT
            API->>DB: Consultar Coordenadas
            API-->>Map View: Lista Coordenadas
        end
    ```

=== "Ejemplos de Respuesta"
    ??? example "Webhook GPS"
        ```json
        {
            "status": 200,
            "message": "Trip added correctly"
        }
        ```
    
    ??? example "Coordenadas App"
        ```json
        {
            "message": "coordinate added successfully"
        }
        ```
    
    ??? example "Consulta Coordenadas"
        ```json
        {
            "results": [
                {
                    "IdViaje": 123,
                    "HoraCoordenada": "2024-03-20T10:30:00",
                    "Latitud": -33.4569,
                    "Longitud": -70.6483,
                    "Velocidad": 60.5,
                    "Patente": "ABCD12"
                }
            ],
            "not_found": []
        }
        ```

??? warning "Consideraciones de Seguridad"
    - Validación de fuentes GPS
    - Autenticación de dispositivos
    - Protección de datos de ubicación
    - Control de acceso a coordenadas
    - Encriptación de comunicaciones

??? tip "Mejores Prácticas"
    1. **Validación de Datos**:
        - Rango de coordenadas válido
        - Timestamps coherentes
        - Velocidades realistas
        - Patentes existentes
    
    2. **Optimización**:
        - Filtrado de coordenadas redundantes
        - Compresión de datos
        - Indexación geoespacial
        - Caché de consultas frecuentes
    
    3. **Monitoreo**:
        - Latencia de webhooks
        - Precisión de coordenadas
        - Frecuencia de actualización
        - Cobertura de señal

??? note "Detalles Técnicos"
    | Aspecto | Especificación |
    |---------|----------------|
    | Formato Coordenadas | Decimal Degrees |
    | Precisión | 6 decimales |
    | Timezone | UTC |
    | Rate Limit | 1 req/seg por dispositivo |
    | Retención | 30 días |

=== "Códigos de Error"
    | Código | Descripción |
    |--------|-------------|
    | 401 | No autorizado |
    | 404 | Viaje no encontrado |
    | 422 | Coordenadas inválidas |
    | 429 | Rate limit excedido |
    | 500 | Error interno |

=== "Integración Mapas"
    - Soporte para múltiples proveedores
    - Clustering de marcadores
    - Polylines para rutas
    - Geofencing
    - Alertas de desvío