# Como utilizarlo 

## Incluir JaCoCo en el proyecto

- 1: En el gradle del proyecto, es decir, en build.gradle(Nombre Proyecto), agregar la version de JaCoCo. Algo asi nos quedaria:


```
buildscript {
    ext.kotlin_version = "1.4.32"
    ext.jacocoVersion = "0.8.4"
    repositories {
        google()
        jcenter()
        mavenCentral()
    }
    dependencies {
        classpath "com.android.tools.build:gradle:4.1.3"
        classpath "org.jetbrains.kotlin:kotlin-gradle-plugin:$kotlin_version"
        // JaCoCo
        classpath "org.jacoco:org.jacoco.core:$jacocoVersion"
	// Other dependencies ...
    }
}
```

- 2: Ahora en el gradle de la app (o del modulo que queramos que ejecute el code coverage), agregamos la configuracion necesaria para que arme los reportes. Estos reportes se van a crear a nivel local, y nos van a dar metricas de cada archivo en particular, test ejecutados, coverage, etc.
En plugins, agregar lo siguiente `id 'jacoco'`.
Luego, agregar la siguiente task:

```
task jacocoCoverageReport(type: JacocoReport, dependsOn: ['testDebugUnitTest', 'createDebugCoverageReport']) {
    reports {
        xml.enabled = true
        html.enabled = true
        def fileFilter = ['**/R.class', '**/R$*.class', '**/BuildConfig.*', '**/Manifest*.*', '**/*Test*.*', 'android/**/*.*']
        def debugTree = fileTree(dir: "${buildDir}/intermediates/classes/debug", excludes: fileFilter)
        def mainSrc = "${project.projectDir}/src/main/java"
        sourceDirectories.setFrom(files([mainSrc]))
        classDirectories.setFrom(files([debugTree]))
        executionData.setFrom(fileTree(dir: "$buildDir", includes: [
                "jacoco/testDebugUnitTest.exec",
                "outputs/code-coverage/connected/coverage.ec"
        ]))
    }
}

tasks.withType(Test) {
    jacoco.includeNoLocationClasses = true
    jacoco.excludes = ['jdk.internal.*']
}
```

Y por ultimo, dentro del objeto android de las configuraciones, agregar estas propiedades:

Dentro de buildTypes tiene que quedar lo siguiente en debug

```
buildTypes {
        // other props
        debug {
            testCoverageEnabled true
        }
}
```

y en testOptions 

```
testOptions {
        unitTests.all {
            jacoco {
                includeNoLocationClasses = true
            }
        }
        unitTests.returnDefaultValues = true
    }
```

- 3: Por ultimo, hacemos sync a estos cambios, y ya podemos correr los reportes. Se pueden correr desde el mismo android studio seleccionando la task, o yendo 
al root del proyecto y en la terminal ejecutar 
```
./gradlew jacocoCoverageReport 
```

## Agregando el script pre-commit:

Una vez que tenemos configurada nuestra task de Code Coverage, tenemos que incluir el **git-hook** de *pre-commit* (o si se quiere pre-push). 
Git automaticamente crea hooks, y estos estan en la carpeta oculta *.git* en la *root* del repositorio. Entonces una vez situados en la root del proyecto, nos dirigimos a ```.git/hooks/```, y sobre-escribimos el actual script de *pre-commit* con el que se creo en este repositorio. 

Luego, como JaCoCo genera archivos html, el hook de pre-commit llama a un script que se encarga de leer el html generado, tomar el porcentaje y escribirlo en un archivo json. Dicho script debe ser ubicado en la root del proyecto de Android Studio. 
El script es *jacocoparser*, y se debe ubicar en la root del proyecto de Android Studio.

# Troubleshooting

- Un error que puede ocurrir (mas que nada en MacOs) es que el gradlew no detecta correctamente la JDK. 
```
> Kotlin could not find the required JDK tools in the Java installation '/Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home' used by Gradle. Make sure Gradle is running on a JDK, not JRE.
```
Para solucionarlo vamos al file gradle.properties, y agregamos lo siguiente al final

```
org.gradle.java.home=/Users/'YOUR USER'/Library/Java/JavaVirtualMachines/'YOUR JAVA VERSION'/Contents/Home
```
