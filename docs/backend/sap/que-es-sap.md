# SAP Business Layer - Documentación

## Descripción General
SAP Business Layer (SAP B1 Service Layer) es una API basada en OData que permite la interacción con SAP Business One a través de servicios web RESTful.

??? info "Características Principales"
    - API basada en OData
    - Servicios web RESTful
    - Autenticación requerida
    - Operaciones CRUD
    - Consultas flexibles

## Estructura Base

```plaintext
https://servidor.com:50000/b1s/v1/
```

Donde:
- `servidor.com:50000`: Host y puerto del servidor
- `/b1s/v1/`: Versión y punto de entrada

## Operaciones Principales

=== "Autenticación"
    ```json
    POST /b1s/v1/Login
    {
        "CompanyDB": "NOMBRE_DB",
        "UserName": "USUARIO",
        "Password": "CONTRASEÑA"
    }
    ```

=== "Consultas Básicas"
    ??? example "Obtener Clientes"
        ```http
        GET /b1s/v1/BusinessPartners
        ```
    
    ??? example "Filtrar por Ciudad"
        ```http
        GET /b1s/v1/BusinessPartners?$filter=City eq 'Santiago'
        ```
    
    ??? example "Seleccionar Campos"
        ```http
        GET /b1s/v1/BusinessPartners?$select=CardCode,CardName,City
        ```

=== "Operaciones CRUD"
    ??? success "Crear (POST)"
        ```json
        POST /b1s/v1/BusinessPartners
        {
            "CardCode": "C001",
            "CardName": "Cliente Ejemplo",
            "City": "Santiago"
        }
        ```
    
    ??? info "Leer (GET)"
        ```http
        GET /b1s/v1/BusinessPartners('C001')
        ```
    
    ??? warning "Actualizar (PATCH)"
        ```json
        PATCH /b1s/v1/BusinessPartners('C001')
        {
            "City": "Valparaíso"
        }
        ```
    
    ??? danger "Eliminar (DELETE)"
        ```http
        DELETE /b1s/v1/BusinessPartners('C001')
        ```

## Parámetros OData

??? tip "Parámetros Principales"
    | Parámetro | Descripción | Ejemplo |
    |-----------|-------------|---------|
    | $filter | Filtrar resultados | `?$filter=City eq 'Santiago'` |
    | $select | Seleccionar campos | `?$select=CardCode,CardName` |
    | $expand | Expandir relaciones | `?$expand=ContactEmployees` |
    | $orderby | Ordenar resultados | `?$orderby=CardName desc` |
    | $top | Limitar resultados | `?$top=10` |
    | $skip | Saltar resultados | `?$skip=20` |

## Ejemplos Prácticos

=== "Consulta de Facturas"
    ```http
    GET /b1s/v1/Invoices?$filter=DocDate ge '2024-01-01'
    ```

=== "Crear Orden de Compra"
    ```json
    POST /b1s/v1/PurchaseOrders
    {
        "CardCode": "P001",
        "DocDate": "2024-03-20",
        "DocumentLines": [
            {
                "ItemCode": "A001",
                "Quantity": 10
            }
        ]
    }
    ```

??? warning "Consideraciones Importantes"
    1. **Seguridad**:
        - Mantener seguras las credenciales
        - Usar HTTPS
        - Renovar tokens periódicamente
    
    2. **Rendimiento**:
        - Optimizar consultas
        - Usar paginación
        - Limitar campos seleccionados
    
    3. **Buenas Prácticas**:
        - Manejar errores apropiadamente
        - Documentar cambios
        - Mantener consistencia en datos

??? note "Recursos Adicionales"
    - [Documentación Oficial SAP](https://help.sap.com/docs/SAP_BUSINESS_ONE)
    - [Guía de OData](https://www.odata.org/documentation/)
    - [Ejemplos de Integración](https://blogs.sap.com/tags/b1-service-layer/)