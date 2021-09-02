# Full build-info MVCE

Generate a build-info with both consumed and produced modules using the 
Artifactory plugin.

# Building 

The artifacts for the `app`, `list`, and `utilities` projects publish to the
`local` repo:

    ./test-repo

The project is setup with a multi-project setup so that all of these modules can be
published via pull through of:

    ./gradlew artifactoryPublish

_NOTE: Due to the fact that this creates project dependencies vs module dependencies it is 
reported differently_

If you want to publish the individual projects, then checkout the `separate` branch:

    git checkout separate
    ./gradlew :list:publishAllPublicationsToLocalRepo
    ./gradlew :utilities:publishAllPublicationsToLocalRepo

Then publish the project that has a transitive dependency:

    ./gradlew artifactoryPublish

# Debug

To list the configurations add:

    afterEvaluate { configurations.forEach { logger.info(it); } }

Then run:

    $ ./gradlew -p ./app/ clean assemble --info

    configuration ':cppCompileDebug'
    configuration ':cppCompileRelease'
    configuration ':cppCompileTest'
    configuration ':debugRuntimeElements'
    configuration ':implementation'
    configuration ':mainDebugImplementation'
    configuration ':mainReleaseImplementation'
    configuration ':nativeLinkDebug'
    configuration ':nativeLinkRelease'
    configuration ':nativeLinkTest'
    configuration ':nativeRuntimeDebug'
    configuration ':nativeRuntimeRelease'
    configuration ':nativeRuntimeTest'
    configuration ':releaseRuntimeElements'
    configuration ':testExecutableImplementation'
    configuration ':testImplementation'

We can see the list of transitive dependencies:

    $ ./gradlew -p ./app/ dependencyInsight --configuration nativeRuntimeRelease --dependency "com.company:list"

    > Task :dependencyInsight
    com.company:list:3.2.1
    variant "releaseRuntime" [
        org.gradle.native.architecture    = x86-64
        org.gradle.native.debuggable      = true
        org.gradle.native.linkage         = SHARED (not requested)
        org.gradle.native.operatingSystem = linux
        org.gradle.native.optimized       = true
        org.gradle.usage                  = native-runtime
        org.gradle.status                 = release (not requested)
    ]

    com.company:list:3.2.1
    \--- com.company:utilities_release:6.5.4
        \--- com.company:utilities:6.5.4
            \--- nativeRuntimeRelease

    com.company:list_release:3.2.1
    variant "releaseRuntime" [
        org.gradle.native.architecture    = x86-64
        org.gradle.native.debuggable      = true
        org.gradle.native.linkage         = SHARED (not requested)
        org.gradle.native.operatingSystem = linux
        org.gradle.native.optimized       = true
        org.gradle.usage                  = native-runtime
        org.gradle.status                 = release (not requested)
    ]

    com.company:list_release:3.2.1
    \--- com.company:list:3.2.1
        \--- com.company:utilities_release:6.5.4
            \--- com.company:utilities:6.5.4
                \--- nativeRuntimeRelease

Attempt to run the build:

    $ ./gradlew -p ./app/ assemble
    > Task :compileDebugCpp UP-TO-DATE
    > Task :linkDebug UP-TO-DATE
    > Task :installDebug
    > Task :assemble

BUILD SUCCESSFUL in 992ms
3 actionable tasks: 1 executed, 2 up-to-date

Linking issue:

    $ ./app/build/install/main/debug/app
    ./app/build/install/main/debug/lib/app: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.21' not found (required by ./app/build/install/main/debug/lib/app)
    ./app/build/install/main/debug/lib/app: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.21' not found (required by ./app/build/install/main/debug/lib/libutilities.so)
    ./app/build/install/main/debug/lib/app: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.21' not found (required by ./app/build/install/main/debug/lib/liblist.so)
    ./app/build/install/main/debug/lib/app: /lib64/libstdc++.so.6: version `CXXABI_1.3.9' not found (required by ./app/build/install/main/debug/lib/liblist.so)

    $ ldd ./app/build/install/main/debug/lib/app
    ./app/build/install/main/debug/lib/app: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.21' not found (required by ./app/build/install/main/debug/lib/app)
            linux-vdso.so.1 =>  (0x00007ffcf1b7f000)
            libutilities.so => not found
            liblist.so => not found
            libstdc++.so.6 => /lib64/libstdc++.so.6 (0x00007f4392f8c000)
            libm.so.6 => /lib64/libm.so.6 (0x00007f4392c8a000)
            libgcc_s.so.1 => /lib64/libgcc_s.so.1 (0x00007f4392a73000)
            libc.so.6 => /lib64/libc.so.6 (0x00007f43926b0000)
            /lib64/ld-linux-x86-64.so.2 (0x00005633b14a2000)

Let's look at our linker arguments:

    $ find . -path "*/tmp/*Debug*/options.txt" -type f
    ./app/build/tmp/compileDebugCpp/options.txt
    ./app/build/tmp/linkDebug/options.txt
    ./list/build/tmp/compileDebugCpp/options.txt
    ./list/build/tmp/linkDebug/options.txt
    ./utilities/build/tmp/compileDebugCpp/options.txt
    ./utilities/build/tmp/linkDebug/options.txt

Examine the options of the app executable and how it was linked:

    $ cat ./app/build/tmp/linkDebug/options.txt
    -o
    /root/full-build-info-mvce/app/build/exe/main/debug/app
    /root/full-build-info-mvce/app/build/obj/main/debug/5v08dtn7cszvzrlgdayn1y9u6/main.o
    /root/full-build-info-mvce/app/build/obj/main/debug/f0c36b8y9kkzr0hshipmuavim/message.o
    /root/full-build-info-mvce/.gradle-user-home/caches/modules-2/files-2.1/com.company/utilities_debug/6.5.4/8162f96f3a7f07df749a669df1cc69c6c7255de6/libutilities.so
    /root/full-build-info-mvce/.gradle-user-home/caches/modules-2/files-2.1/com.company/list_debug/3.2.1/703e2bfb87017d2e9c5b3a3c6a966e41991b1f06/liblist.so
    -m64

List what the utilities library was linked with:

    $ ldd /root/full-build-info-mvce/.gradle-user-home/caches/modules-2/files-2.1/com.company/utilities_debug/6.5.4/8162f96f3a7f07df749a669df1cc69c6c7255de6/libutilities.so
    ldd: warning: you do not have execution permission for `/root/full-build-info-mvce/.gradle-user-home/caches/modules-2/files-2.1/com.company/utilities_debug/6.5.4/8162f96f3a7f07df749a669df1cc69c6c7255de6/libutilities.so'
    /root/full-build-info-mvce/.gradle-user-home/caches/modules-2/files-2.1/com.company/utilities_debug/6.5.4/8162f96f3a7f07df749a669df1cc69c6c7255de6/libutilities.so: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.21' not found (required by /root/full-build-info-mvce/.gradle-user-home/caches/modules-2/files-2.1/com.company/utilities_debug/6.5.4/8162f96f3a7f07df749a669df1cc69c6c7255de6/libutilities.so)
            linux-vdso.so.1 =>  (0x00007fffa2dc8000)
            liblist.so => not found
            libstdc++.so.6 => /lib64/libstdc++.so.6 (0x00007fa88f52e000)
            libm.so.6 => /lib64/libm.so.6 (0x00007fa88f22c000)
            libgcc_s.so.1 => /lib64/libgcc_s.so.1 (0x00007fa88f016000)
            libc.so.6 => /lib64/libc.so.6 (0x00007fa88ec52000)
            /lib64/ld-linux-x86-64.so.2 (0x0000563b41218000

Examine the options of the utilities library and how it was linked:

    $ cat ./utilities/build/tmp/linkDebug/options.txt
    -shared
    -Wl,-soname,libutilities.so
    -o
    /root/full-build-info-mvce/utilities/build/lib/main/debug/libutilities.so
    /root/full-build-info-mvce/utilities/build/obj/main/debug/3aemsj805oudnd40y4s4r3zup/split_and_join.o
    /root/full-build-info-mvce/.gradle-user-home/caches/modules-2/files-2.1/com.company/list_debug/3.2.1/703e2bfb87017d2e9c5b3a3c6a966e41991b1f06/liblist.so
    -m64

Let's see what the compiler is referencing:

    $ which gcc
    /custom/gcc-6.2.0/bin/gcc

Ah! Need to add the lib to this:

    export LD_LIBRARY_PATH=/custom/gcc-6.2.0/lib64:${LD_LIBRARY_PATH}

That still did not work when calling the Gradle default loader:

    $ cat ./app/build/install/main/debug/app
    #!/bin/sh
    APP_BASE_NAME=`dirname "$0"`
    DYLD_LIBRARY_PATH="$APP_BASE_NAME/lib"
    export DYLD_LIBRARY_PATH
    LD_LIBRARY_PATH="$APP_BASE_NAME/lib"
    export LD_LIBRARY_PATH
    exec "$APP_BASE_NAME/lib/app" "$@"

Instead just manually set this up:

    export LD_LIBRARY_PATH=/custom/gcc-6.2.0/lib64:/root/full-build-info-mvce/app/build/install/main/debug/lib/:${LD_LIBRARY_PATH}
     $ ./app/build/install/main/debug/lib/app
     Token: 'thisDEBUG isDEBUG theDEBUG tokenDEBUG

    

