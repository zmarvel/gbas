pipeline {
    agent any
    stages {
        stage('Git checkout') {
            steps{
                checkout scm
            }
        }
        stage('Build GCC') {
            steps {
                sh 'make clean'
                sh 'make CXX=g++'
            }
        }
        stage('Test GCC') {
            steps {
                sh 'make CHECK_OPTIONS="--log_format=JUNIT --log_level=all" CHECK_LOG=test_log_gcc.xml check'
            }
        }
        stage('Build Clang') {
            steps {
                sh 'make clean'
                sh 'make CXX=clang++'
            }
        }
        stage('Test Clang') {
            steps {
                sh 'make CXX=clang++ CHECK_OPTIONS="--log_format=JUNIT --log_level=all" CHECK_LOG=test_log_clang.xml check'
            }
        }
    }
}
