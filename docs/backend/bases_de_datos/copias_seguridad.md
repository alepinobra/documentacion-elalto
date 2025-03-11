# 📚 Copias de seguridad DB Azure

## 📄 Creación y Restauración de Copias de Seguridad en Azure

??? info "Introducción"
   - Esta documentación describe el proceso de creación y  restauración de copias de seguridad en Azure para bases de datos SQL, incluyendo PITR (Point-in-Time Restore) y copias de seguridad a largo plazo (LTR).

## 🚀 Restaurar DB

### 1️⃣ Creación de Copias de Seguridad

??? tip "Configuración"
    1.1. Copias de seguridad automáticas:

        - ✅ Azure SQL Database realiza copias de seguridad de manera automática sin intervención manual.
        
    1.2. Configurar la retención de copias de seguridad:

        - ✅ Iniciar sesión en el portal de Azure: https://portal.azure.com
        - ✅ Ir a Azure SQL Database y seleccionar la base de datos.
        - ✅ En informacion general, ir a Propiedades.
        - ✅ En Propiedades presionar "Copias de seguridad".
        - ✅ Dentro de copias presionar "Directivas de retención".
        - ✅ Presionar "Configurar directivas" y ajustar la configuración según lo que se necesite.

### 2️⃣ Restauración de Copias de Seguridad
   -  Restauración desde un punto en el tiempo (PITR)


=== "Método Manual"
    ```powershell
    - ✅ Ir a Azure SQL Database y seleccionar la base de datos.
    - ✅ En el menú superior, hacer clic en Restaurar.
    - ✅ Elegir la fecha y la hora exacta del punto de restauración.
    - ✅ Definir un nuevo nombre para la base de datos restaurada.
    - ✅ Confirmar y esperar la restauración.
    ```

=== "Método PowerShell"
    ```powershell
    Restore-AzSqlDatabase -FromPointInTimeBackup
    -ResourceGroupName "MiGrupo"
    -ServerName "MiServidor" 
    -TargetDatabaseName "MiBD_Restaurada"
    -PointInTime "2024-03-10T12:00:00Z"
    ```

## ⚙️ Consideraciones Importantes

  - ✅ Tiempo de restauración: La restauración puede tardar minutos u horas dependiendo del tamaño de la base de datos.
  - ✅ Restauración en un nuevo nombre: La base restaurada siempre tendrá un nombre diferente al original (renombrar nueva DB).
  - ✅ Renombrar la DB nueva y eliminar la antigua.
  - ✅Copias LTR y PITR: Si necesitas restaurar datos anteriores al rango de PITR, usa LTR.
  


## ⚠️ Solución de Problemas

??? warning "Problemas Comunes"
    | Problema | Solución |
    |----------|----------|
    |La restauración lleva mucho tiempo o parece detenida. | Las restauraciones de bases de datos grandes pueden tardar varias horas. Puedes ver el progreso en Portal de Azure → Monitorización. |
    | No puedo restaurar la base de datos por falta de permisos. | Asegúrate de tener el rol Owner o SQL DB Contributor en el recurso de Azure. |
    | La base restaurada no contiene los datos esperados. | Revisa el punto de restauración y selecciona otro más reciente. |
    | La opción de restauración a un punto en el tiempo no aparece o no permite seleccionar una fecha. | Verifica en Portal de Azure → Copia de seguridad si el punto de restauración está disponible. |

## 📚 Recursos

??? info "Enlaces Útiles"
    - [Portal Azure](https://portal.azure.com/)
    - [Documentacion copias de seguridad](https://learn.microsoft.com/es-es/azure/azure-sql/database/automated-backups-overview?view=azuresql)
  