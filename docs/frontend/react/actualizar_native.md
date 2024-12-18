# 🔄 Actualizaciones de la App

### Proceso de Actualización

??? info "Generar Nueva Versión"
    1. **Actualizar Versión**
        ```json
        // app.json
        {
          "expo": {
            "name": "Mi App",
            "version": "1.0.1", // Incrementar versión
            "android": {
              "versionCode": 2 // Incrementar código
            }
          }
        }
        ```

    2. **Generar nuevo build**
        ```bash
        # Generar nuevo .aab
        eas build -p android
        ```

### 📱 Subir Actualización a Play Store

??? example "Paso a Paso"
    1. Acceder a [Google Play Console](https://play.google.com/console)
    2. Seleccionar tu aplicación
    3. Navegar a `Production` en el menú lateral
    4. Click en `Create new release`
    5. Subir el archivo `.aab` generado por Expo
    6. Completar notas de la versión
    7. Revisar y enviar para revisión

??? tip "Detalles del Proceso"
    | Paso | Descripción | Tiempo Estimado |
    |------|-------------|-----------------|
    | Subir .aab | Archivo generado por `eas build` | 5-10 min |
    | Revisión Google | Proceso automático/manual | 1-3 días |
    | Distribución | Disponible en Play Store | 1-2 horas |

### ✅ Lista de Verificación para Updates

??? check "Antes de Subir"
    - [ ] Incrementar `version` en app.json
    - [ ] Incrementar `versionCode` en app.json
    - [ ] Probar app en modo release
    - [ ] Preparar notas de la versión
    - [ ] Generar nuevo .aab
    - [ ] Verificar cambios importantes

### 📝 Notas de la Versión

??? example "Ejemplo de Notas"
    ```markdown
    Versión 1.0.1:
    • Corrección de errores menores
    • Mejoras de rendimiento
    • Nuevas características:
      - Feature 1
      - Feature 2
    ```

### ⚠️ Consideraciones Importantes

??? warning "Puntos a Tener en Cuenta"
    - No se puede bajar la versión una vez publicada
    - El `versionCode` debe ser mayor al anterior
    - La revisión puede requerir información adicional
    - Mantener respaldo de versiones anteriores
    - Considerar actualizaciones graduales

### 🔍 Monitoreo Post-Actualización

??? tip "Métricas a Observar"
    - Tasa de adopción de la nueva versión
    - Reportes de errores
    - Comentarios de usuarios
    - Desinstalaciones
    - Crashes en la nueva versión

### 🆘 Solución de Problemas Comunes

??? warning "Problemas y Soluciones"
    | Problema | Solución |
    |----------|----------|
    | Rechazo de actualización | Revisar políticas de Google |
    | Error en .aab | Regenerar build |
    | Versión incorrecta | Verificar app.json |
    | Distribución lenta | Esperar 24h |

### 📊 Seguimiento de Versiones

??? example "Registro de Actualizaciones"
    ```markdown
    # Historial de Versiones
    
    ## v1.0.1 (2024-03-20)
    - Build: 2
    - Changes: Bug fixes
    
    ## v1.0.0 (2024-03-01)
    - Build: 1
    - Changes: Initial release
    ```