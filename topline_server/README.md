topline-server
==============

JavaEE code for topline server

Java Development Kit, Apache Tomcat server and Apache Ant are required.

* Download [JDK](http://www.oracle.com/technetwork/java/javase/downloads/index.html) and install it.

* set `CLASSPATH` and `PATH` for Java

* Download [Apache Tomcat](http://tomcat.apache.org/). No installation needed, just unzip it.

* Download [Apache Ant](http://ant.apache.org/). Unzip it and set `PATH` for ant.

* Copy the binary file, configuration files and related dll into `WebContent/tools`.

* Change the two line in `build.xml` according to your tomcat.

```XML
<property name="tomcat.lib" value="D:/apache-tomcat-7.0.56/lib" />
<property name="webserver" value="D:/apache-tomcat-7.0.56/webapps"/>
```

* Use `ant` to generate the war, it should be in target folder

