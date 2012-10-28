# Tatodb  Graph database for Tatoeba sentences database


Tatodb is a database written in C specifically made to handle the graph representation of the sentences in Tatoeba
in order to provide something way more efficient than using a SQL database

This project is composed of 

  * libtato: the library permitting you to embed this database 
  * server: an http server implementing the libtato and exposing an http API
  

#TODO

Write documentation, build instruction etc.


**Note**: the project also contains an "hypergraph" version of the code (and of the server), it will be moved to a dedicated repository in the future
