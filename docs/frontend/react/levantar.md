# 🚀 Guía de Instalación: Next.js + Shadcn/UI

## 📋 Prerequisitos

??? info "Requisitos del Sistema"
    - Node.js 18.17 o superior
    - pnpm, npm, yarn o bun
    - Editor de código (VS Code recomendado)

## 🛠️ Instalación

### 1️⃣ Crear Proyecto Next.js

=== "Con pnpm"
    ```bash
    pnpm create next-app my-app
    cd my-app
    ```

=== "Con npm"
    ```bash
    npx create-next-app my-app
    cd my-app
    ```

=== "Con yarn"
    ```bash
    yarn create next-app my-app
    cd my-app
    ```

### 2️⃣ Configurar Shadcn/UI

??? example "Inicialización"
    ```bash
    # Método Interactivo
    pnpm dlx shadcn@latest init
    
    # Método con Defaults (-d)
    pnpm dlx shadcn@latest init -d
    ```

??? tip "Preguntas de Configuración"
    | Pregunta | Opciones | Recomendado |
    |----------|----------|-------------|
    | Style | Default/New York | New York |
    | Color Base | Slate/Zinc/... | Zinc |
    | CSS Variables | Yes/No | Yes |

### 3️⃣ Instalar Componentes

=== "Método Individual"
    ```bash
    # Instalar un componente
    pnpm dlx shadcn@latest add button
    
    # Instalar múltiples componentes
    pnpm dlx shadcn@latest add card form input
    ```

=== "Componentes Populares"
    ```bash
    # UI Básica
    pnpm dlx shadcn@latest add button card input form dialog
    
    # Navegación
    pnpm dlx shadcn@latest add navigation-menu dropdown-menu
    
    # Datos
    pnpm dlx shadcn@latest add table select combobox
    ```

## 💻 Uso de Componentes

??? example "Ejemplo Básico"
    ```tsx
    import { Button } from "@/components/ui/button"
    import { Card } from "@/components/ui/card"
    
    export default function Home() {
      return (
        <Card>
          <Button>Click me</Button>
        </Card>
      )
    }
    ```

??? example "Formulario"
    ```tsx
    import { Button } from "@/components/ui/button"
    import { Input } from "@/components/ui/input"
    import { Form } from "@/components/ui/form"
    
    export default function LoginForm() {
      return (
        <Form>
          <Input placeholder="Email" />
          <Input type="password" placeholder="Password" />
          <Button>Login</Button>
        </Form>
      )
    }
    ```

## 🎨 Personalización

??? tip "Temas"
    ```tsx
    // globals.css
    @layer base {
      :root {
        --background: 0 0% 100%;
        --foreground: 240 10% 3.9%;
        /* Añadir más variables según necesidad */
      }
    }
    ```

??? tip "Componentes"
    ```tsx
    // components/ui/custom-button.tsx
    import { Button } from "@/components/ui/button"
    import { cn } from "@/lib/utils"
    
    export function CustomButton({ className, ...props }) {
      return (
        <Button 
          className={cn("bg-primary text-white", className)} 
          {...props} 
        />
      )
    }
    ```

## ✅ Verificación

??? check "Lista de Verificación"
    - [ ] Next.js instalado y funcionando
    - [ ] Shadcn/UI inicializado
    - [ ] Componentes necesarios instalados
    - [ ] Temas configurados
    - [ ] Estructura de carpetas correcta

## ⚠️ Solución de Problemas

??? warning "Problemas Comunes"
    | Problema | Solución |
    |----------|----------|
    | Componente no encontrado | Verificar instalación |
    | Estilos no aplicados | Revisar globals.css |
    | TypeScript errors | Actualizar tipos |
    | Build fails | Limpiar cache y node_modules |

## 📚 Recursos

??? info "Enlaces Útiles"
    - [Documentación Shadcn/UI](https://ui.shadcn.com)
    - [Next.js Docs](https://nextjs.org/docs)
    - [Ejemplos de Componentes](https://ui.shadcn.com/docs/components)
    - [GitHub Repo](https://github.com/shadcn/ui)