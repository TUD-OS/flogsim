DROP USER IF EXISTS 'user'@'localhost';
FLUSH PRIVILEGES;
CREATE USER 'user'@'localhost' IDENTIFIED BY 'user';

DROP DATABASE IF EXISTS flogsim;
CREATE DATABASE flogsim;
USE flogsim;

DROP TABLE IF EXISTS experiment_plan;
DROP TABLE IF EXISTS experiment_log;

CREATE TABLE experiment_plan (
    id INT NOT NULL AUTO_INCREMENT,
    COLL VARCHAR(40) NOT NULL,
    k INT,
    L INT,
    o INT,
    g INT,
    P INT,
    F INT,
    conducted INT,
    total INT,
    primary key(id));

CREATE TABLE experiment_log (
    COLL VARCHAR(40),
    k INT,
    L INT,
    o INT,
    g INT,
    P INT,
    F INT,
    runtime INT,
    failed_nodes INT,
    finished_nodes INT,
    unreached_nodes INT,
    msg_task INT,
    seed INT);

GRANT SELECT, INSERT, UPDATE ON flogsim.* TO 'user';
