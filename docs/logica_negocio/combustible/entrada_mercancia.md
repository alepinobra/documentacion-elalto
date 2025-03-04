# API Combustible - Documentación (Admin)

## Descripción General
Sistema de gestión de combustible que permite registrar y consultar el consumo de combustible de la flota. Acceso exclusivo para administradores.

??? info "Notas Importantes"
    - Sistema CRUD para gestión de combustible
    - Acceso restringido a administradores
    - Modelos principales:
        ```python title="modelos"
        class reqSession(BaseModel):
            Folio: str
            
        class reqSessionPostStock(BaseModel):
            Folio: str
            Planta: str
            Patente: str
            FotoDocumento: str  # Base64
            WarehouseCodeQuantity: List[WarehouseCodeQuantityItem]
            UnitPrice: float
            
        class WarehouseCodeQuantityItem(BaseModel):
            WarehouseCode: str
            Quantity: float
        ```
??? warning "Consideraciones de Seguridad"
    - Validación de key de autorización
    - Acceso restringido a administradores
    - Validación de documentos
    - Protección de datos sensibles
    - Control de acceso a bodegas

??? tip "Mejores Prácticas"
    - Validar documentos antes de procesar
    - Mantener registro de transacciones
    - Monitorear niveles de stock
    - Documentar cambios de inventario
    - Notificar eventos importantes

??? note "Integraciones"
    - **FEBOS**:
        - Consulta de documentos tributarios
        - Validación de guías
    - **SAP**:
        - Control de inventario
        - Registro de movimientos
    - **Azure**:
        - Almacenamiento de documentos
        - Gestión de archivos
    - **Email**:
        - Notificaciones automáticas
        - Alertas de stock

## Endpoints Combustible

=== "Endpoints"
    ??? success "Consultar Guía de Combustible"
        ```python title="POST /api/logipath/get_fuel_guide"
        @router.post(f"{APP_PATH}/get_fuel_guide")
        async def fuel(res: Request, req: reqSession):
            """
            Obtiene información detallada de una guía de combustible.
            
            Args:
                res (Request): Request con key de autorización
                req (reqSession): Folio de la guía
                
            Returns:
                dict: {
                    "documento": dict,
                    "tipo_dte": str,
                    "folio": int,
                    "fecha_emision": str,
                    "razon_social_emisor": str,
                    "monto_total": float,
                    "cantidad": float,
                    "PrcItem": float,
                    "DirDest": str,
                    "CmnaDest": str,
                    "CiudadDest": str
                }
                
            Raises:
                HTTPException: 
                    - 401: Si la key es inválida
                    - 500: Error en la consulta
            """
        ```
    
    ??? success "Consultar Stock de Combustible"
        ```python title="POST /api/logipath/get_stock_fuel"
        @router.post(f"{APP_PATH}/get_stock_fuel")
        async def fuel(res: Request, req: reqSession):
            """
            Obtiene el stock actual de combustible por bodega.
            
            Args:
                res (Request): Request con key de autorización
                req (reqSession): Datos de consulta
                
            Returns:
                dict: {
                    "bodegas": [
                        {
                            "WarehouseCode": str,
                            "cantidad": float,
                            "WarehouseName": str,
                            "MaximalStock": float,
                            "MinimalStock": float
                        }
                    ]
                }
            """
        ```
    
    ??? success "Registrar Consumo de Combustible"
        ```python title="POST /api/logipath/post_fuel_stock"
        @router.post(f"{APP_PATH}/post_fuel_stock")
        async def fuel(res: Request, req: reqSessionPostStock):
            """
            Registra el consumo de combustible y actualiza stock.
            
            Args:
                res (Request): Request con key de autorización
                req (reqSessionPostStock): Datos del consumo
                
            Returns:
                dict: {
                    "message": str,
                    "IdAzure": str,
                    "LinkAzure": str
                }
            """
        ```

=== "Sequence Diagram"
    ```mermaid
    sequenceDiagram
        participant Admin
        participant API
        participant FEBOS
        participant SAP
        participant Azure
        participant Email
        
        alt Consulta Guía
            Admin->>API: POST /get_fuel_guide
            API->>API: Validar Key
            API->>FEBOS: Consultar Guía
            FEBOS-->>API: XML Guía
            API->>API: Procesar XML
            API-->>Admin: Datos Guía
            
        else Consulta Stock
            Admin->>API: POST /get_stock_fuel
            API->>API: Validar Key
            API->>SAP: Login
            SAP-->>API: Token
            API->>SAP: Consultar Stock
            SAP-->>API: Datos Stock
            API-->>Admin: Stock por Bodega
            
        else Registrar Consumo
            Admin->>API: POST /post_fuel_stock
            API->>API: Validar Key
            API->>SAP: Actualizar Stock
            
            par Almacenar Documento
                API->>Azure: Subir Foto
                Azure-->>API: URL Documento
            and Notificar
                API->>Email: Enviar Notificación
            end
            
            API-->>Admin: Confirmación
        end
    ```

=== "Ejemplos de Respuesta"
    ??? example "Consulta Guía"
        ```json
        {
            "documento": {
                "xmlLink": "https://api.febos.cl/..."
            },
            "tipo_dte": "33",
            "folio": "123456",
            "fecha_emision": "2024-03-20",
            "razon_social_emisor": "Empresa Combustibles",
            "monto_total": 150000.00,
            "cantidad": 100.00,
            "PrcItem": 1500.00
        }
        ```
    
    ??? example "Consulta Stock"
        ```json
        {
            "bodegas": [
                {
                    "WarehouseCode": "BOD1017",
                    "cantidad": 5000.00,
                    "WarehouseName": "Estanque Equipos Móviles",
                    "MaximalStock": 10000.00,
                    "MinimalStock": 1000.00
                }
            ]
        }
        ```
=== "Códigos de Estado"
    | Código | Descripción |
    |--------|-------------|
    | 200 | Operación exitosa |
    | 401 | No autorizado |
    | 404 | Documento no encontrado |
    | 500 | Error interno |

=== "Validaciones"
    | Campo | Regla |
    |-------|-------|
    | Folio | Formato válido |
    | Cantidad | > 0 |
    | FotoDocumento | Base64 válido |
    | WarehouseCode | Existente en SAP |