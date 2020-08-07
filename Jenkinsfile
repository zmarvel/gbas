pipeline {
    agent { dockerfile true }
    stages {
        stage('Git checkout') {
            steps{
                checkout scm
            }
        }
        stage('Build GCC') {
            steps {
                sh 'rm -r build.gcc'
                sh 'mkdir build.gcc && cd build.gcc && cmake ..'
                sh 'cd build.gcc && make -j4 gbas'
            }
        }
        stage('Test GCC') {
            steps {
                sh 'cd build.gcc && make -j4 gbas_test'
                sh 'cd build.gcc && ./gbas_test --log_format=JUNIT --log_level=all > test_log_gcc.xml'
            }
        }
        stage('Build Clang') {
            steps {
                sh 'rm -r build.clang'
                sh 'mkdir build.clang && cd build.clang && CXX=clang++ CC=clang cmake ..'
                sh 'cd build.clang && make -j4 gbas'
            }
        }
        stage('Test Clang') {
            steps {
                sh 'cd build.clang && make -j4 gbas_test'
                sh 'cd build.clang && ./gbas_test --log_format=JUNIT --log_level=all > test_log_gcc.xml'
            }
        }
    }
}
