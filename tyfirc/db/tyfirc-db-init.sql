CREATE DATABASE IF NOT EXISTS tyfirc;

USE tyfirc;

CREATE TABLE IF NOT EXISTS users (
	userid INT AUTO_INCREMENT PRIMARY KEY,
	username VARCHAR(255) UNIQUE NOT NULL,
	password VARCHAR(255) NOT NULL
);

CREATE USER 'tyfirc-server'
IDENTIFIED WITH mysql_native_password BY 'password';

GRANT INSERT, SELECT, UPDATE, DELETE
ON tyfirc.*
TO 'tyfirc-server';