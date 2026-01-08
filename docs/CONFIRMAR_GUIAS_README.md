# Documentación de Confirmación de Guías

## 📚 Archivos Creados

### Backend
- **Ubicación:** `docs/backend/tracmin/guías/confirmar-guias.md`
- **Contenido:** Documentación completa del sistema de confirmación de guías en el backend (AppLogiPath)

### Frontend
- **Ubicación:** `docs/frontend/tracmin/confirmar-guias.md`
- **Contenido:** Documentación completa del sistema de confirmación de guías en el frontend (LogiPath)

## 🎯 Descripción

La documentación cubre el proceso completo de confirmación de guías de despacho, incluyendo:

### Backend (AppLogiPath)
- Endpoints de API para obtener y confirmar guías
- Lógica de validación de pesos (Guía vs Ticket)
- Manejo de casos especiales:
  - Devoluciones (Peso Guía > Peso Ticket)
  - Nuevas Guías (Peso Ticket > Peso Guía)
  - Confirmación Normal
  - Rectificaciones
- Integración con SAP Service Layer
- Sistema de caché de sesiones SAP
- Estados de guías y control de procesamiento

### Frontend (LogiPath)
- Interfaz de usuario para confirmación masiva
- Componentes React y TypeScript
- Sistema de selección inteligente de guías
- Modal de confirmación manual con validaciones
- Manejo de tickets objetados
- Cálculo automático de diferencias de peso
- Visualización de estado de confirmación
- Flujos de usuario detallados

## 🔗 Referencias Cruzadas

La documentación incluye enlaces a:
- Crear Guías
- Cancelar Guías
- Correo de Guías
- Documentación de SAP Service Layer
- TanStack Table v8
- ShadcnUI Components

## 📋 Contenido Destacado

### Diagramas
- ✅ Flujo de trabajo completo (Backend)
- ✅ Flujo de confirmación masiva (Frontend)
- ✅ Flujo de confirmación individual (Frontend)
- ✅ Manejo de tickets objetados (Frontend)

### Código de Ejemplo
- ✅ Endpoints de API con parámetros completos
- ✅ Lógica de negocio para diferentes casos
- ✅ Componentes React completos
- ✅ Tipos TypeScript
- ✅ Validaciones del cliente

### Guías de Troubleshooting
- ✅ Problemas comunes y soluciones (Backend)
- ✅ Problemas de UI y soluciones (Frontend)

## 🚀 Cómo Usar

1. **Ver localmente:**
   ```bash
   cd documentacion-elalto
   mkdocs serve
   ```
   
2. **Acceder a:**
   - Backend: http://localhost:8000/backend/tracmin/guías/confirmar-guias/
   - Frontend: http://localhost:8000/frontend/tracmin/confirmar-guias/

3. **Construir documentación:**
   ```bash
   mkdocs build
   ```

## 📝 Actualizaciones Realizadas

- [x] Crear documentación de backend
- [x] Crear documentación de frontend
- [x] Actualizar mkdocs.yml con nuevas páginas
- [x] Añadir diagramas de flujo con Mermaid
- [x] Incluir ejemplos de código
- [x] Documentar API routes de Next.js
- [x] Describir componentes y su arquitectura
- [x] Añadir sección de troubleshooting

## 🔍 Información Técnica

### Backend
- **Lenguaje:** Python
- **Framework:** FastAPI
- **Base de datos:** SQL Server
- **SAP:** Service Layer API
- **Autenticación:** JWT + Header Authorization

### Frontend
- **Framework:** Next.js 14
- **Lenguaje:** TypeScript
- **UI Library:** ShadcnUI + Tailwind CSS
- **Tabla:** TanStack Table v8
- **Estado:** React Hooks (useState, useCallback, useMemo)
- **Notificaciones:** SweetAlert2

## 📧 Contacto

Para dudas o sugerencias sobre la documentación, contactar al equipo de desarrollo.

---

**Última actualización:** 30 de diciembre de 2025
