/*
* Analizador Léxico 
* Materia: Compiladores y Lenguajes de Bajo Nivel
* Tarea N° 1.
*
* Descripcion:
* Implementar un analizador léxico que reconozca numeros, comentarios y operadores.
* Implementar un evaluador de expresiones aritmeticas. 
*/

/*********** Librerias utilizadas **************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include <conio.h>



/***************** MACROS **********************/

// Codigos
#define OPSUMA		100
#define OPMULT		101
#define OPDIVI		102
#define OPREST		103
#define NUM		    104
#define ID		    105

// Fin Codigos

#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101
#define TAMSTACK 	100
#define MAXIMO 30

/************* Definiciones ********************/

typedef struct entrada{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	
	struct entrada *tipoDato; // null puede representar variable no declarada	
	// aqui irian mas atributos para funciones y procedimientos...
	
} entrada;

typedef struct {
	int compLex;
	entrada *pe;
} token;


typedef struct LIFO {
	int t;
	char *a[MAXIMO];
};


struct NODO
{
    double valor;
    struct NODO *puntero;
};


/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
char infijo[TAMSTACK][TAMSTACK]; //utilizado para la expresion aritmetica de cada linea del archivo
char postfijo[TAMSTACK][TAMSTACK]; //notacion postfija
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente pascal
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
int nuevaLinea=0;      // Indica si el analizador lexico pasa a la sgte linea

/************** Prototipos *********************/

void sigLex();		// Del analizador Lexico
void in_a_pos(char infijo[TAMSTACK][TAMSTACK], char posfijo[TAMSTACK][TAMSTACK]); // convierte de notacion infija a postfija
int operando (char c[TAMLEX]); //retorna 1 si es un operando, 0 si es un operador.
int prioridad (char op1[2],char op2[2]); //obtiene la prioridad entre operadores a partir de una matriz
char* tope (struct LIFO p); //obtiene el elemento que se encuantra en el tope de la PILA
void init_pila (struct LIFO *p); //inicializa los elementos de la PILA
int pila_vacia (struct LIFO *p); //verifica si la PILA esta vacia
void ins_pila (struct LIFO *p,char s[TAMLEX]); //push
char* retira_pila (struct LIFO *p); //pop


/**************** Funciones **********************/
void printInfijo(int ptrInfijo){
     int i, j;
     for(i=0; i<ptrInfijo; i++){
           printf("Elemento %s\n", infijo[i]);   
     }
}

void printPosfijo(){
     int i, j;
     for(i=0; i<TAMSTACK; i++){
           printf("Elemento %s\n", postfijo[i]);   
     }
}

int stricmp(const char* cad, const char* cad2) 
{
	int i;
	char c1[strlen(cad)];
	char c2[strlen(cad2)];
	
	strcpy(c1,cad);
	strcpy(c2,cad2);
	for(i=0;i<strlen(c1);i++)
		c1[i]=tolower(c1[i]);

	for(i=0;i<strlen(c2);i++)
		c2[i]=tolower(c2[i]);

	return strcmp(c1,c2);
}

void initInfijo(){
    int i, j;
    for(i=0; i<TAMSTACK; i++){
        for(j=0; j<TAMSTACK; j++){
          strcpy(infijo[i],"");     
        }     
    } 
}

void initPostfijo(){
    int i, j;
    for(i=0; i<TAMSTACK; i++){
        for(j=0; j<TAMSTACK; j++){
            strcpy(postfijo[i],"");     
        }     
    }
}

char* invertir(char cadena[TAMLEX]){
      char i,j;
      char longitud;   
      char temporal;
      longitud=strlen(cadena);
      for (i=0,j=longitud-1; i<longitud/2; i++,j--)
      {
           temporal=cadena[i];
	       cadena[i]=cadena[j];
	       cadena[j]=temporal;
      }
      return cadena;
}


int operando (char c[TAMLEX])
{
	return ( strcmp(c,"+")!=0 &&
		   strcmp(c,"-")!=0 &&
		   strcmp(c,"*")!=0 &&
		   strcmp(c,"/")!=0 &&
		   strcmp(c,"^")!=0 &&
		   strcmp(c,")")!=0 &&
		   strcmp(c,"(")!=0    );
}

static int m[6][7] = {
	{ 1,1,0,0,0,0,1 },
	{ 1,1,0,0,0,0,1 },
	{ 1,1,1,1,0,0,1 },
	{ 1,1,1,1,0,0,1 },
	{ 1,1,1,1,1,0,1 },
	{ 0,0,0,0,0,0,0 }
};

int prioridad (char op1[2],char op2[2]){
    
    
	int i,j;
	
	if(strcmp(op1,"+")==0){
       i = 0; 
    }else if(strcmp(op1,"-")==0){
       i = 1; 
    }else if(strcmp(op1,"*")==0){
       i = 2; 
    }else if(strcmp(op1,"/")==0){
       i = 3; 
    }else if(strcmp(op1,"^")==0){
       i = 4; 
    }else if(strcmp(op1,"(")==0){
       i = 5; 
    }
	
	if(strcmp(op2,"+")==0){
       j = 0; 
    }else if(strcmp(op2,"-")==0){
       j = 1; 
    }else if(strcmp(op2,"*")==0){
       j = 2; 
    }else if(strcmp(op2,"/")==0){
       j = 3; 
    }else if(strcmp(op2,"^")==0){
       j = 4; 
    }else if(strcmp(op2,"(")==0){
       j = 5; 
    }else if(strcmp(op2,")")==0){
       j = 6; 
    }
    
    //printf("prioridad \n");
    	
	return (m [i][j]);
}

char* tope (struct LIFO p){
    return (p.a [p.t -1]);
}


void init_pila (struct LIFO *p){
	p->t = 0;
}

int pila_vacia (struct LIFO *p){
	return (!p->t);
}

void ins_pila (struct LIFO *p,char s[TAMLEX]){
	if (p->t == MAXIMO)
		printf ("la pila no soporta mas elementos\n");
	else  {
		  p->t++;
          p->a [p->t - 1] = s;
	}
}

char* retira_pila (struct LIFO *p)
{
    char s[TAMLEX];
      
	if (pila_vacia (p) )  {
		printf ("la pila esta vacia\n");
		//*s = "#";
		strcpy(s,"#");
	}
	else  {
		  //*s = p->a [p->t - 1];
		  //printf("se retira de la pila este elemento %s \n", p->a [p->t - 1]);
		  strcpy(s, p->a [p->t - 1]);
		  p->t--;
		  //printf("se retira de la pila este elemento %s \n", s);
	}
	//printf("retira pila \n");
	return s;
}


void in_a_pos (char infijo[TAMSTACK][TAMSTACK], char posfijo[TAMSTACK][TAMSTACK]){
     
     
     
    //declaraciones 
	struct LIFO pila;
	int i,j;
	char elemento[TAMLEX];
    
    //asiganciones
	i=0;
	j=-1;
	
	//inicializamos los elementos de la pila
	init_pila (&pila);
    
    //printf("inicializo la pila\n");
    
	while (strcmp(infijo[i],"#")!=0) {
       //printf("elemento verificado %s\n", infijo[i]);   
	   if (operando(infijo [i])){
	      // si es un operando incluirlo a la cadena postfijo
	      //printf("paso uno\n");
		   strcpy(posfijo[++j],infijo[i++]);
	   }else {
            //es un operador
            //printf("paso dos\n");
		     while (!pila_vacia (&pila)  &&
       			 prioridad (tope (pila),infijo [i] ) )  {
                 //mientras la pila no este vacia y la prioridad del operador del tope de la PILA es mayor que el operador
                 //del infijo
                 //printf("dentro del while\n");
			     strcpy(elemento,retira_pila(&pila));
			     //printf("elemento retira pila OK! %s\n", elemento);
			     strcpy(posfijo[++j],elemento);
		      }

		      if (strcmp(infijo[i],")")==0)		         
			     strcpy(elemento,retira_pila(&pila));
		      else{
                  ins_pila(&pila,infijo [i]);
                  //printf("inserto en la pila %s\n", infijo[i]); 
              } 
                  
           i++;
		}

	}

	while (!pila_vacia (&pila) ) {
       strcpy(elemento,retira_pila(&pila));
	   strcpy(posfijo [++j],elemento);
	}

	strcpy(posfijo [++j],"\0");
	
	//printPosfijo();
}


/*********************HASH************************/
entrada *tabla;				//declarar la tabla de simbolos
int tamTabla=TAMHASH;		//utilizado para cuando se debe hacer rehash
int elems=0;				//utilizado para cuando se debe hacer rehash

int h(const char* k, int m)
{
	unsigned h=0,g;
	int i;
	for (i=0;i<strlen(k);i++)
	{
		h=(h << 4) + k[i];
		if (g=h&0xf0000000){
			h=h^(g>>24);
			h=h^g;
		}
	}
	return h%m;
}
void insertar(entrada e);

void initTabla()
{	
	int i=0;
	
	tabla=(entrada*)malloc(tamTabla*sizeof(entrada));
	for(i=0;i<tamTabla;i++)
	{
		tabla[i].compLex=-1;
	}
}

int esprimo(int n)
{
	int i;
	for(i=3;i*i<=n;i+=2)
		if (n%i==0)
			return 0;
	return 1;
}

int siguiente_primo(int n)
{
	if (n%2==0)
		n++;
	for (;!esprimo(n);n+=2);

	return n;
}

//en caso de que la tabla llegue al limite, duplicar el tamaño
void rehash()
{
	entrada *vieja;
	int i;
	vieja=tabla;
	tamTabla=siguiente_primo(2*tamTabla);
	initTabla();
	for (i=0;i<tamTabla/2;i++)
	{
		if(vieja[i].compLex!=-1)
			insertar(vieja[i]);
	}		
	free(vieja);
}

//insertar una entrada en la tabla
void insertar(entrada e)
{
	int pos;
	if (++elems>=tamTabla/2)
		rehash();
	pos=h(e.lexema,tamTabla);
	while (tabla[pos].compLex!=-1)
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	tabla[pos]=e;

}
//busca una clave en la tabla, si no existe devuelve NULL, posicion en caso contrario
entrada* buscar(const char *clave)
{
	int pos;
	entrada *e;
	pos=h(clave,tamTabla);
	while(tabla[pos].compLex!=-1 && stricmp(tabla[pos].lexema,clave)!=0 )
	{
		pos++;
		if (pos==tamTabla)
			pos=0;
	}
	return &tabla[pos];
}

void insertTablaSimbolos(const char *s, int n)
{
	entrada e;
	sprintf(e.lexema,s);
	e.compLex=n;
	insertar(e);
}

void initTablaSimbolos()
{
	insertTablaSimbolos("+",OPSUMA);
	insertTablaSimbolos("-",OPREST);
	insertTablaSimbolos("*",OPMULT);
	insertTablaSimbolos("/",OPDIVI);
    insertTablaSimbolos("(",'(');
	insertTablaSimbolos(")",')');
}

// Rutinas del analizador lexico

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}

void sigLex()
{
	int i=0, longid=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;
	
	while((c=fgetc(archivo))!=EOF)
	{
		
		if (c==' ' || c=='\t')
			continue;	//eliminar espacios en blanco
		else if(c=='\n')
		{
			//incrementar el numero de linea
			numLinea++;
			nuevaLinea=1; //nueva linea
			continue;
		}
		else if (isalpha(c))
		{
			//es un identificador (o palabra reservada)
			i=0;
			do{
				id[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c) || isdigit(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1)
			{
				sprintf(e.lexema,id);
				e.compLex=ID;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=ID;
			}
			break;
		}
		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (t.pe->compLex==-1)
						{
							sprintf(e.lexema,id);
							e.compLex=NUM;
							insertar(e);
							t.pe=buscar(id);
						}
						t.compLex=NUM;
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		else if (c=='/')
		{
			if ((c=fgetc(archivo))=='/')
			{//es un comentario
				while(c!=EOF)
				{
					c=fgetc(archivo);
					if (c=='\n')
					{
						break;
					}
				}
				if (c==EOF)
					error("Se llego al fin de archivo sin finalizar un comentario");
				continue;
			}
			else
			{
				ungetc(c,archivo);
				t.compLex='/';
				t.pe=buscar("/");
			}
			break;
		}
		else if (c=='(')
		{
			t.compLex='(';
			t.pe=buscar("(");
			break;
		}
		else if (c==')')
		{
			t.compLex=')';
			t.pe=buscar(")");
			break;
		}
		else if (c=='+')
		{
			t.compLex=OPSUMA;
			t.pe=buscar("+");
			break;
		}
		else if (c=='-')
		{
			t.compLex=OPREST;
			t.pe=buscar("-");
			break;
		}
		else if (c=='*')
		{
			t.compLex=OPMULT;
			t.pe=buscar("*");
			break;
		}
		else if (c=='/')
		{
			t.compLex=OPDIVI;
			t.pe=buscar("/");
			break;
		}
		
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

int esNroNotacionCientifica(char nro[TAMLEX], int longitud){
    int i;
    int raton = 0;
    for(i=0; i<longitud; i++){
        if(nro[i] == 'E' || nro[i] == 'e'){
             raton = 1;
        }         
    }
    return raton;
}


double convierteStringToDouble(char nro[TAMLEX]){
       double x;
       char *p;
       x = strtod (nro, &p);
       return x;
}


// FUNCIÓN QUE INTRODUCE UN NUMERO EN UNA PILA:
void insertaDatoEnLaPila( struct NODO **pila, double num )
{
    struct NODO *temp = (struct NODO *) malloc(sizeof(struct NODO));
    temp->valor        = num;
    temp->puntero    = *pila;
    *pila            = temp;
 
};
 

int pilaVacia (struct NODO **pila){
	return (!(*pila)->valor);
}

void initPila (struct NODO **pila){
     struct NODO *temp  = (struct NODO *) malloc(sizeof(struct NODO));
     temp->valor        = 0;
     temp->puntero      = *pila;
     *pila              = temp;
}
 
// FUNCIÓN QUE EXTRAE EL PRIMER ELEMENTO DE LA PILA Y BORRA EL NODO VACIO:
double extraeDatoDeLaPila( struct NODO **pila )
{
    struct NODO *siguiente;
    double resp;
    //printf("valor en extraeDatoDeLaPila: %d\n",(*pila)->valor);      
 
    // Se copia el valor a la variable que va a ser retornada por la función:
    resp = (*pila)->valor;
 
 
    // Se elimina el primer Nodo (el vaciado):
    siguiente = (*pila)->puntero;
    free(*pila);
    *pila = siguiente;
 
 
    return resp;
};
 
 
 
// FUNCIÓN QUE LIBERA LA MEMORIA USADA POR UNA LISTA:
void listaLiberaRam( struct NODO **lista )
{
  
    struct NODO *actual, *siguiente;
    actual = *lista;
 
    while(actual != NULL)
    {
        siguiente = actual->puntero;
        free(actual);
        actual = siguiente;
    };
 
    *lista = NULL;
 
};


char* calcularPostfijo(char postfijo[TAMSTACK][TAMSTACK]){
    struct NODO *pila;
    double val1, val2;
    char resultado[100];
    char resultDoubleString[50];
    int posf;
    
    initPila(&pila);
    
    //calcula el resulta de la notacion polaca inversa
	           for(posf=0; posf<TAMSTACK; posf++){
                     if(strcmp(postfijo[posf],"\0")!=0){
                           //printf("aqui.. se procesa %s\n", postfijo[posf]);                             
                          // Dependiendo de lo leido se hace una operacion u otra:
                          if(strcmp(postfijo[posf],"+")==0){
                              // En el caso de la: '+', '-', '*' y '/', se sacan dos valores de la pila, se opera con ellos según el operador elegido y se guarda el resultado en la pila.
                              val1 = extraeDatoDeLaPila( &pila );
                              //printf("val1 %f\n", val1);
                              if(!pilaVacia(&pila)){
                                  val2 = extraeDatoDeLaPila( &pila );                  
                                  insertaDatoEnLaPila( &pila, val2 + val1 );                       
                              }else{
                                 strcpy(resultado,"Error en la expresion");
                                 break;                        
                              } 
                          }else if(strcmp(postfijo[posf],"-")==0){
                                val1 = extraeDatoDeLaPila( &pila );
                                if(!pilaVacia(&pila)){
                                  val2 = extraeDatoDeLaPila( &pila );                  
                                  insertaDatoEnLaPila( &pila, val2 - val1 );                       
                                }else{
                                  strcpy(resultado,"Error en la expresion");
                                  break;                        
                                }
                                //printf("valor val2 %f\n", val2);
                          }else if(strcmp(postfijo[posf],"*")==0){
                                val1 = extraeDatoDeLaPila( &pila );
                                if(!pilaVacia(&pila)){
                                  val2 = extraeDatoDeLaPila( &pila );                  
                                  insertaDatoEnLaPila( &pila, val2 * val1 );                       
                                }else{
                                 strcpy(resultado,"Error en la expresion");
                                 break;                        
                                }
                          }else if(strcmp(postfijo[posf],"/")==0){
                                val1 = extraeDatoDeLaPila( &pila );
                                if(!pilaVacia(&pila)){
                                  val2 = extraeDatoDeLaPila( &pila );                  
                                  insertaDatoEnLaPila( &pila, val2 / val1 );                       
                                }else{
                                  strcpy(resultado,"Error en la expresion");
                                  break;                        
                                }
                          }else{
                                // Se introduce un nuevo dato en la pila:
                                insertaDatoEnLaPila( &pila, convierteStringToDouble(postfijo[posf]));
                                //printf("insertar %s\n", postfijo[posf]);      
                          }   
                      }else{
                            // Se imprime por pantalla el ultimo y único elemento de la pila.
                           sprintf(resultDoubleString, "%f", extraeDatoDeLaPila( &pila )); 
                           strcpy(resultado, resultDoubleString); 
                           break;
                      }      
               }
               return resultado;
               //   
}


/*
*
*   METODO MAIN()
*
*/

int main(int argc,char* args[])
{
	// inicializar analizador lexico
	int complex=0;
	int i=0;
	int ptrInfijo = -1;
	char lexem[TAMLEX];
	int posf;
	int unaSolaExp = 0;
	char resultado[100];
	char exp[100];
	
	initTabla();
	initTablaSimbolos();
	initInfijo();
	initPostfijo();
	strcpy(exp, "");
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no tencontrado.\n");
			exit(1);
		}
		while (t.compLex!=EOF){
			sigLex();
			//sgte linea del archivo
			if(nuevaLinea == 1){
               //convierte de notacion infija a postfija
               strcpy(infijo[++ptrInfijo],"#");
               strcpy(infijo[++ptrInfijo],"\0");
               in_a_pos(infijo,postfijo);
               //initPila(&pila);
               //printInfijo(ptrInfijo);
               //printf("*********************\n");
               //printPosfijo();
	           strcpy(resultado,calcularPostfijo(postfijo));
	           printf("El resultado de la expresion (%s) es: %s\n", exp, resultado);
               initInfijo();
               ptrInfijo = -1;
               initPostfijo();
               nuevaLinea = 0;
               //listaLiberaRam(&pila);
               ++unaSolaExp;
               strcpy(exp, "");
            }
            //printf("Lin %d: %s -> %d\n",numLinea,t.pe->lexema,t.compLex);
            if(t.compLex != -1){
               strcat(exp, t.pe->lexema);                                               
               if(esNroNotacionCientifica(t.pe->lexema, strlen(t.pe->lexema))){                                    
                   //sprintf(DString,"%lf",D);
                   sprintf(lexem,"%lf",convierteStringToDouble(t.pe->lexema));                                     
                   strcpy(infijo[++ptrInfijo],lexem);
               }else{
                   strcpy(infijo[++ptrInfijo],t.pe->lexema);
                   //printf("lexema %s \n", t.pe->lexema);
               }          
            }
		}
		
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
        
    getch();
	return 0;
}




