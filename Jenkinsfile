pipeline {
    agent { dockerfile true }
    stages {
        stage('Build GCC') {
            steps {
                sh 'mkdir build.gcc && cd build.gcc && cmake ..'
                sh 'cd build.gcc && make -j4 gbas'
            }
        }
        stage('Test GCC') {
            steps {
                sh 'cd build.gcc && make -j4 gbas_test'
                catchError {
                    sh 'cd build.gcc/test && ./gbas_test --log_format=JUNIT --log_level=all > test_log_gcc.xml'
                }
            }
        }
        stage('Build Clang') {
            steps {
                sh 'mkdir build.clang && cd build.clang && CXX=clang++ CC=clang cmake ..'
                catchError {
                    sh 'cd build.clang/test && make -j4 gbas'
                }
            }
        }
        stage('Test Clang') {
            steps {
                sh 'cd build.clang && make -j4 gbas_test'
                catchError {
                    sh 'cd build.clang/test && ./gbas_test --log_format=JUNIT --log_level=all > test_log_gcc.xml'
                }
            }
        }
    }
}
