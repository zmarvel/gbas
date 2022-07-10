pipeline {
    agent { dockerfile true }
    stages {
        stage('Build GCC') {
            steps {
                sh 'cmake -S . -B build.gcc -DCMAKE_BUILD_TYPE=RelWithDebInfo -G Ninja'
                sh 'cmake --build build.gcc'
            }
        }

        stage('Test GCC') {
            steps {
                sh 'cmake --build build.gcc --target gbas_test'
                catchError {
                    sh 'cd build.gcc && ./test/gbas_test --log_format=JUNIT --log_level=all > test_log_gcc.xml'
                }
            }
        }

        stage('Build Clang') {
            steps {
                sh 'cmake -S . -B build.clang -DCMAKE_BUILD_TYPE=RelWithDebInfo -G Ninja -DCMAKE_CXX_COMPILER=/usr/bin/clang++'
                sh 'cmake --build build.clang'
            }
        }

        stage('Test Clang') {
            steps {
                sh 'cmake --build build.clang --target gbas_test'
                catchError {
                    sh 'cd build.clang && ./test/gbas_test --log_format=JUNIT --log_level=all > test_log_clang.xml'
                }
            }
        }
    }
}
