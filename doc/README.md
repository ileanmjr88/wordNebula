# WordNebula Documenation

## Overview Design
```mermaid
graph LR
    Model <---> Presenter <---> View
    Model <---> Buffer
    Model <---> Filesystem
    Presenter ---> Formatting 
```