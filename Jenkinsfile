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
                sh 'make CXX=g++ check'
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
                sh 'make CXX=clang++ check'
            }
        }
    }
}
