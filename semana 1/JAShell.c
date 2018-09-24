/* Programa [JASHELL.c]
   Versión  [5.57]
   Autores  [Jose Alberto Benítez Andrades \/ infjab02@estudiantes.unileon.es]
	    [Alejandro Fernández Aparicio \/ infafa02@estudiantes.unileon.es]
   Fecha    [04/05/2007 - 15/05/2007]
   Fuente	https://www.jabenitez.com/portfolio/interprete-de-comandos-en-c-shell/
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define compara " "
#define MSJ_ERROR "Error de sintaxis\n"
#define MAX 51
#define PROMPT "#@> "

int CadIguales(char cadena1[MAX],char cadena2[MAX]);
void Comando(char cadena[MAX], int plano);
void redirecEntrada(char cadena[MAX]);
void redirecSalida(char cadena[MAX]);
void tuberias(char* argumento1[MAX], char* argumento2[MAX]);
void CrearProceso(char* argumento[MAX], int plano);


/* La función principal main contiene el bucle que se ejecutará de forma contínua hasta que
el usuario escriba FIN en el terminal.Recibe una cadena, la analiza, y a partir de las 
funciones que hemos creado, hará lo que debe de hacer */
int main(int argc, char *argv[]) { /* INICIO DE LA FUNCIÓN main */
	char cadena[MAX];  
	char cadFin[]="FIN";
	int fin = 0, i, segplano=0, guardaStdout = dup(1), guardaStdin = dup(0);

	while(fin == 0)  {
	    close(1); /* Cierro la salida que tenga, caso de haber guardado algo en un fichero
	                 será el fichero*/
	    dup(guardaStdout); // Asigno la salida estándar, es decir, la consola.
	    close(0); /* Cierro la entrada que tenga, caso de haber sido algo en un fichero
	                 será el fichero*/
	    dup(guardaStdin); // Asigno la entrada estándar, es decir, la consola.
		printf(PROMPT); // Imprimimos el prompt
		scanf("\n%[^\n]",cadena); // Escaneamos la cadena entera hasta que pulsa intro
		segplano = 0;
		for(i = 0;cadena[i] != '\0'; i++){
			if(cadena[i] == '&') {
				cadena[i] = '\0';
				segplano = 1;
			}
		}
		fin = CadIguales(cadena,cadFin); // Comparamos si la cadena es fin, en tal caso sale
		Comando(cadena,segplano); // Y si no es fin, entra en la función Comando
	}
			
	exit(0);//finaliza el programa (cadena 'FIN' introducida)
	return(0);
} /* FIN DE LA FUNCIÓN main */

/* Con esta función compararemos la cadena que introduce el usuario, con FIN, 
en caso de ser iguales, saldrá del programa, de lo contrario devolverá 0  y el programa
seguirá en ejecución */
int CadIguales(char cadena1[],char cadena2[]) {//inicio del CadIguales
	int i=0;
	int igual=0;

	while ((cadena1[i]==cadena2[i])&&(cadena1[i]!='\0')&&(cadena2[i]!='\0')) i++;	
	if ((cadena1[i]==cadena2[i])&&(cadena1[i]=='\0')&&(cadena2[i]=='\0')) exit(0);				
	return(igual);
}//fin CadIguales

void Comando(char cadena[MAX], int plano) { /* INICIO DE LA FUNCIÓN COMANDO */
	int i,j,k,flag_tuberia;		
	char argumentoInd[MAX][MAX]; 				
	char argumentoInd2[MAX][MAX];
	char redirec[MAX];
	char entrada[MAX];
	char *argumento[MAX]; 
	char *argumento2[MAX];
	int ejecutar=0; //¿por que le puso "ejecutar" si en realidad es un flag de error de parseo?
	argumento[0] = NULL;					
	argumento2[0] = NULL;
	flag_tuberia = 0;

	k = 0;
	i = 0;
	/* Comienza el parseo con un while que busca el comando a ejecutar. Si es que tiene un "<" para redireccionar, lo aplica altoquemon.
	Si tiene algun caracter que indica que hay que hacer otra cosa, hace otra cosa y se sale del while.
	*/
	while(cadena[i] != '\0' && cadena[i] != '|' && cadena[i] != '>') { /* INICIO DE WHILE */	 
		/* Con este for recorremos la cadena por completo, y la vamos almacenando
		en una variable que consiste en un array bidimensional de tipo char, 
		es decir almacena cadenas de cadenas de caracteres */
		for(j=0; cadena[i] != ' ' && cadena[i] != '\0' && cadena[i] != '|'  && cadena[i] != '>' && cadena[i] != '<';j++) { 
			argumentoInd[k][j] = cadena[i];
			i++;
		}

		/* Comprobamos si la cadena salió del bucle por encontrar un espacio...
		en tal caso se incrementa la i, ya que sino entraría en un bucle 
		infinito*/
		if (cadena[i] == ' ') i++;

		/* Asignamos el terminador de cadena a cada comando que es leído */
		argumentoInd[k][j] = '\0';

		/* Y finalmente una vez creada la cadena, se la pasamos al puntero 
		argumento que será el que se ejecute con la función execvp */
		argumento[k] = argumentoInd[k];
		k++;

		if (cadena[i] == '<') { /* INICIO DEL IF */
			i++;
			if (cadena[i] != ' ') ejecutar = 1;
			else { 
				i++;
				for(j=0; cadena[i] != '\0' && cadena[i] != ' ' && cadena [i] != '|' && cadena [i] != '>'; j++) {
					entrada[j] = cadena[i];
					i++;
				}
				entrada[j] = '\0';
				if (cadena[i] != '\0') i++;
				redirecEntrada(entrada);
			}
		}/* FIN DEL IF */					
	} /* FIN DEL WHILE */

	argumento[k] = NULL; // Asignamos NULL al último comando a ejecutar para el EXECV
		    
	/* Si encuentra una tubería, entrará en el IF y separará el segundo comando de la 
	misma forma que se hizo con el primero, y finalmente llamará a la función tuberias,
	pasándole los 2 argumentos como parámetros */
	if (cadena[i] == '|') {/* INICIO DE IF */
		k=0;
		i++;
		if (cadena[i] != ' ') ejecutar = 1;
		else { /* INICIO DEL ELSE */
			i++;
			flag_tuberia = 1;
			while(cadena[i] != '\0' && cadena[i] != '>') {/* INICIO DE WHILE */	 
				for(j=0; cadena[i] != ' ' && cadena[i] != '\0' && cadena[i] != '>';j++) {
					argumentoInd2[k][j] = cadena[i];
					i++;
				}
				if (cadena[i] == ' ' ) i++;
				argumentoInd2[k][j] = '\0';
				argumento2[k] = argumentoInd2[k];
				k++;
			}/* FIN DEL WHILE */
			argumento2[k] = NULL;
		}/*FIN EL ELSE */		
	}/* FIN DE IF */

	/* Si encuentra un > cortará la cadena que será el fichero que quiere utilizar
	para la salida */
	if (cadena[i] == '>') {/* INICIO DE IF */
		i++;
		if (cadena[i] != ' ') ejecutar = 1;
		else {
			i++;
			for(j=0; cadena[i] != '\0';j++) {
				redirec[j] = cadena[i];
				i++;
			}
			redirec[j] = '\0';
			redirecSalida(redirec);	
		}
	}/* FIN DE IF */
			
	/* Comprobamos si la variable ejecutar tiene valor 0 o 1, si tiene valor 0
		el programa se ejecutará correctamente, si tiene valor 1 significa que 
		mientras analizaba alguna de las cadenas ha encontrado un error de sintaxis
		y mostrará en pantalla un mensaje de error. */	
	if(ejecutar == 0) {
		if (flag_tuberia==0) CrearProceso(argumento,plano);
		else tuberias(argumento,argumento2);
	}
	else printf( MSJ_ERROR );	
}/* FIN DE LA FUNCIÓN COMANDO */


/* Con esta función si el usuario introduce quiere cambiar la entrada estándar
( el teclado ) por lo que hay en un fichero, podrá hacerlo mediante el símbolo 
"<", por ejemplo: wc < fichero */
void redirecEntrada(char cadena[MAX]) { /* INICIO DE LA FUNCIÓN redirecEntrada */
  char *cadenaPtr;
  int fd;  
  cadenaPtr = cadena; // Puntero a la cadena
  fd = open (cadenaPtr,O_RDONLY); // Asigno a la salida el fichero
  close (0); // Cierro la salida estándar	
  dup(fd);	
} /* FIN DE LA FUNCIÓN redirecEntrada */

/* Con esta función si el usuario introduce una redirección de salida a un 
fichero, en lugar de mostrar el comando en pantalla, lo guardará en fichero
por ejemplo: man -k file > lista.file */
void redirecSalida(char cadena[MAX]) { /* INICIO DE LA FUNCIÓN redirecSalida */
  char *cadenaPtr;
  cadenaPtr = cadena; // Puntero a la cadena
  close (1); // Cierro la salida estándar		
  open (cadenaPtr,O_CREAT | O_WRONLY,0777); // Asigno a la salida el fichero
} /* FIN DE LA FUNCIÓN redirecSalida */

/* Con esta función en el caso de que haya tuberías ( se ejecuten dos comandos 
concatenados) por ejemplo: ls -la | sort , recibirá como parámetro argumento1 y 
argumento2 de tipo puntero a cadena de cadena de caracteres y creará un hijo que
será el encargado de ejecutar los 2 comandos pasados por tubería.*/
void tuberias(char* argumento1[MAX], char* argumento2[MAX]) { /* INICIO DE LA FUNCIÓN tuberias */
	int fd[2],estado;
	pid_t hijo; 
	hijo=fork();

	if (hijo == -1) printf("ERROR Creacion de proceso"); 
	else if (hijo == 0) {
		pipe(&fd[0]); /* Función pipe encargada de crear la tubería */
   		if (fork() != 0) {
      		close(fd[0]); /* cerramos el lado de lectura del pipe */
      		close(1);
      		dup(fd[1]); /* STDOUT = extremo de salida del pipe */
      		close(fd[1]);/* cerramos el descriptor de fichero que sobra 
							tras el dup(fd[1])*/
      		execvp(argumento1[0],argumento1);
   		} else {
      		close(fd[1]);/* cerramos el lado de lectura del pipe */
      		close(0);
      		dup(fd[0]);/* STDOUT = extremo de salida del pipe */
      		close(fd[0]);/* cerramos el descriptor de fichero que sobra 
							tras el dup(fd[0])*/
     		execvp(argumento2[0],argumento2);
    	}
	}
	else  hijo = wait(&estado);
} /* FIN DE LA FUNCIÓN tuberias */

/* Con esta función en el caso de que no haya tuberías y sea un sólo comando a ejecutar,
por ejemplo: ls -la , recibirá como parámetro el argumento de tipo puntero a cadena
de cadena de caracteres, creará un hijo para que el comando execvp no finalice el
programa y en definitiva ejecutará el comando que le pedimos.*/
void CrearProceso(char* argumento[MAX],int plano) {	/* INICIO DE LA FUNCIÓN CrearProceso */
	int estado = 0;
	pid_t hijo; 
	hijo = fork();
	/* Comprobamos si el hijo se creó bien */
	if (hijo == -1) printf("ERROR Creacion de proceso"); 
	else if (hijo == 0) {
		/* Y en caso de que el hijo esté bien creado, ejecutamos el proceso,
		si el programa comando a ejecutar no existe, nos da un error.*/
		execvp(argumento[0],argumento);
		perror("JASHELL");
		exit(estado);
	}
	else { 
		if (plano == 0) hijo = wait(&estado);
	}
} /* FIN DE LA FUNCIÓN CrearProceso */

