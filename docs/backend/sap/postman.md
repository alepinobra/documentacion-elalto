# Guía de Postman para SAP Business Layer

## Gestión de Cookies

??? info "Cookies Importantes"
    ```makefile
    B1SESSION: 40e92be0-9142-11ef-8000-fa163ec60aa5
    ROUTEID: .node2
    ```

    | Cookie | Descripción | Uso |
    |--------|-------------|-----|
    | B1SESSION | Token de sesión | Identificación de sesión activa |
    | ROUTEID | Nodo del servidor | Balance de carga |

## Configuración en Postman

=== "Autenticación"
    ```json
    POST https://servidor:50000/b1s/v1/Login
    {
        "CompanyDB": "EMPRESA_DB",
        "UserName": "USUARIO",
        "Password": "CONTRASEÑA"
    }
    ```

=== "Variables de Entorno"
    ```javascript
    // Pre-request Script
    pm.environment.set("servidor", "mi-sap-server");
    pm.environment.set("puerto", "50000");
    ```

=== "Tests Automáticos"
    ```javascript
    // Tests
    pm.test("Guardar B1SESSION", function() {
        pm.environment.set("B1SESSION", 
            pm.cookies.get("B1SESSION"));
    });
    ```

## Consultas OData

??? example "Filtros Básicos"
    ```http
    GET {{base_url}}/Orders?$filter=DocEntry eq 123
    ```

??? example "Ordenamiento"
    ```http
    GET {{base_url}}/Orders?$orderby=DocEntry desc
    ```

??? example "Paginación"
    ```http
    GET {{base_url}}/Orders?$skip=20&$top=10
    ```

## Joins y Consultas Complejas

=== "$crossjoin Básico"
    ```http
    GET {{base_url}}/$crossjoin(Drafts,Drafts/DocumentLines)
    ?$expand=Drafts($select=DocEntry),
             Drafts/DocumentLines($select=ItemCode)
    ```

=== "Filtros en Joins"
    ```http
    GET {{base_url}}/$crossjoin(...)
    ?$filter=Drafts/DocEntry eq Drafts/DocumentLines/DocEntry
    ```

## Colección de Ejemplos

??? tip "Pedidos Recientes"
    ```http
    GET {{base_url}}/Orders
    ?$filter=DocDate ge '2024-01-01'
    &$orderby=DocDate desc
    &$top=10
    ```

??? tip "Detalles de Cliente"
    ```http
    GET {{base_url}}/BusinessPartners('C001')
    ?$expand=ContactEmployees
    ```

## Manejo de Errores

=== "Códigos Comunes"
    | Código | Descripción | Solución |
    |--------|-------------|----------|
    | 401 | No autorizado | Renovar sesión |
    | 404 | No encontrado | Verificar ruta |
    | 500 | Error interno | Contactar soporte |

=== "Script de Retry"
    ```javascript
    // Pre-request Script
    if (pm.response.code === 401) {
        pm.sendRequest({
            url: pm.environment.get("login_url"),
            method: 'POST',
            header: {'Content-Type': 'application/json'},
            body: {
                mode: 'raw',
                raw: JSON.stringify(loginData)
            }
        });
    }
    ```

## Mejores Prácticas

??? warning "Consideraciones"
    1. **Sesión**:
        - Guardar cookies automáticamente
        - Manejar expiración
        - Renovar sesión cuando sea necesario
    
    2. **Rendimiento**:
        - Limitar resultados con $top
        - Usar selección de campos
        - Optimizar joins
    
    3. **Seguridad**:
        - No compartir cookies
        - Usar variables de entorno
        - Verificar certificados SSL

??? tip "Consejos"
    - Organizar colecciones por módulo
    - Documentar ejemplos
    - Usar variables de entorno
    - Implementar tests automáticos
    - Mantener historial de cambios