# 📱 Guía de Desarrollo React Native con Expo

## 🎯 Prerequisitos

??? info "Requisitos del Sistema"
    - Node.js (versión LTS)
    - VS Code o editor preferido
    - Expo Go (App móvil)
    - Cuenta de Google Play Console
    - macOS, Linux, o Windows (PowerShell/WSL2)

## 🛠️ Configuración Inicial

### 1️⃣ Crear Cuenta Google Play Console

??? tip "Registro en Google Play Console"
    1. Visitar [Google Play Console](https://play.google.com/console)
    2. Pagar tarifa única de $25 USD
    3. Completar información de la cuenta:
        - Datos personales/empresa
        - Información de pago
        - Verificación de identidad

### 2️⃣ Instalación de Herramientas

=== "Expo CLI"
    ```bash
    # Instalar Expo CLI globalmente
    npm install -g expo-cli
    
    # Verificar instalación
    expo --version
    ```

=== "Expo Go App"
    1. Descargar Expo Go:
        - [Android Play Store](https://play.google.com/store/apps/details?id=host.exp.exponent)
        - [iOS App Store](https://apps.apple.com/app/expo-go/id982107779)

## 🚀 Crear Nuevo Proyecto

### 3️⃣ Inicializar Proyecto

=== "Crear Proyecto"
    ```bash
    # Crear nuevo proyecto
    npx create-expo-app@latest MiApp
    
    # Navegar al directorio
    cd MiApp
    ```

=== "Estructura de Archivos"
    ```plaintext
    MiApp/
    ├── app/
    │   ├── index.tsx
    │   └── _layout.tsx
    ├── assets/
    │   └── images/
    ├── package.json
    └── app.json
    ```

## 💻 Desarrollo

### 4️⃣ Configuración Básica

??? example "App.json"
    ```json
    {
      "expo": {
        "name": "Mi Aplicación",
        "slug": "mi-app",
        "version": "1.0.0",
        "orientation": "portrait",
        "icon": "./assets/icon.png",
        "splash": {
          "image": "./assets/splash.png",
          "backgroundColor": "#ffffff"
        }
      }
    }
    ```

### 5️⃣ Ejecutar Aplicación

=== "Desarrollo Local"
    ```bash
    # Iniciar servidor de desarrollo
    npx expo start
    ```

=== "Opciones de Ejecución"
    | Tecla | Acción |
    |-------|---------|
    | a | Abrir Android |
    | i | Abrir iOS |
    | w | Abrir Web |
    | r | Recargar |
    | m | Menú |

## 📱 Pruebas en Dispositivo

??? tip "Usando Expo Go"
    1. Abrir Expo Go en el dispositivo
    2. Escanear QR del terminal
    3. La app se cargará automáticamente

??? example "Ejemplo Básico"
    ```tsx
    import { Text, View, StyleSheet } from 'react-native';
    
    export default function App() {
      return (
        <View style={styles.container}>
          <Text style={styles.text}>
            ¡Hola Mundo!
          </Text>
        </View>
      );
    }
    
    const styles = StyleSheet.create({
      container: {
        flex: 1,
        backgroundColor: '#25292e',
        alignItems: 'center',
        justifyContent: 'center',
      },
      text: {
        color: '#fff',
      },
    });
    ```

## 📦 Publicación en Google Play

### 6️⃣ Preparación para Producción

??? info "Requisitos Google Play"
    - Icono de app (512x512)
    - Screenshots (mínimo 2)
    - Descripción corta y larga
    - Política de privacidad
    - Clasificación de contenido

??? example "Generar APK"
    ```bash
    # Configurar eas.json
    eas build:configure
    
    # Crear build para Android
    eas build -p android
    ```

### 7️⃣ Proceso de Publicación

1. Acceder a Google Play Console
2. Crear nueva aplicación
3. Completar ficha de Play Store
4. Subir APK/Bundle
5. Publicar en producción

## ⚠️ Solución de Problemas

??? warning "Problemas Comunes"
    | Problema | Solución |
    |----------|----------|
    | Metro bundler error | Limpiar cache |
    | Expo Go no conecta | Verificar red |
    | Build falla | Revisar eas.json |
    | APK rechazado | Revisar políticas |

## 📚 Recursos

??? info "Enlaces Útiles"
    - [Documentación Expo](https://docs.expo.dev)
    - [React Native Docs](https://reactnative.dev)
    - [Google Play Console](https://play.google.com/console)
    - [Guías de Publicación](https://developer.android.com/distribute)



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