# Modo Borrador (Draft Mode) en SAP

## Descripción General
El Modo Borrador es una característica que permite validar operaciones sin realizar cambios en la base de datos, ideal para pruebas y validaciones.

??? info "Características Principales"
    - No modifica la base de datos
    - Valida operaciones
    - Simula respuestas
    - Permite pruebas seguras
    - Disponible en múltiples servicios

## Implementación

=== "Sintaxis Básica"
    ```http
    GET /endpoint?draft=true
    ```

=== "Ejemplo Completo"
    ```http
    GET /b1s/v1/BusinessPartners('C001')?draft=true
    ```

## Casos de Uso

??? example "Creación de Objetos Temporales"
    ```json
    POST /b1s/v1/BusinessPartners?draft=true
    {
        "CardCode": "C001",
        "CardName": "Cliente Prueba",
        "CardType": "cCustomer"
    }
    ```

??? example "Validación de Relaciones"
    ```http
    GET /b1s/v1/BusinessPartners('C001')/ContactEmployees?draft=true
    ```

??? example "Actualización Simulada"
    ```json
    PATCH /b1s/v1/BusinessPartners('C001')?draft=true
    {
        "Phone1": "123456789"
    }
    ```

## Servicios Compatibles

=== "Módulos Principales"
    | Módulo | Disponibilidad | Operaciones |
    |--------|---------------|-------------|
    | Business Partners | ✅ Total | CRUD |
    | Documentos | ✅ Total | CRUD |
    | Inventario | ✅ Total | CRUD |
    | Compras | ✅ Total | CRUD |
    | Ventas | ✅ Total | CRUD |

## Validaciones

??? tip "Qué se Valida"
    1. **Estructura de Datos**:
        - Campos requeridos
        - Tipos de datos
        - Longitudes máximas
    
    2. **Reglas de Negocio**:
        - Relaciones entre entidades
        - Restricciones de valores
        - Dependencias
    
    3. **Permisos**:
        - Autorizaciones
        - Roles de usuario
        - Accesos a módulos

## Ejemplos Prácticos

=== "Crear Pedido Borrador"
    ```json
    POST /b1s/v1/Orders?draft=true
    {
        "CardCode": "C001",
        "DocDueDate": "2024-03-20",
        "DocumentLines": [
            {
                "ItemCode": "A001",
                "Quantity": 10
            }
        ]
    }
    ```

=== "Validar Relaciones"
    ```http
    GET /b1s/v1/$crossjoin(Orders,Orders/DocumentLines)
    ?draft=true
    &$expand=Orders($select=DocEntry),
             Orders/DocumentLines($select=ItemCode)
    ```

??? warning "Consideraciones Importantes"
    1. **Limitaciones**:
        - No genera números de documento
        - No actualiza contadores
        - No crea logs de auditoría
    
    2. **Rendimiento**:
        - Similar al modo normal
        - No impacta en tiempos de respuesta
        - No consume recursos adicionales
    
    3. **Seguridad**:
        - Requiere mismos permisos
        - Mantiene trazabilidad
        - No compromete datos

??? tip "Mejores Prácticas"
    1. **Uso Recomendado**:
        - Desarrollo de integraciones
        - Pruebas de validación
        - Verificación de reglas
        - Simulación de escenarios
    
    2. **Documentación**:
        - Documentar pruebas realizadas
        - Mantener casos de prueba
        - Registrar resultados
    
    3. **Mantenimiento**:
        - Actualizar casos de prueba
        - Revisar cambios en reglas
        - Mantener scripts actualizados