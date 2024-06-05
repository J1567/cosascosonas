#include <stdio.h>
#include <stdlib.h>
#include "pila.h"
#include <string.h>

const MAX_MESAS = 50;

typedef struct
{
    char Nombre[100];
    char Telefono[15];
} Cliente;
typedef struct
{
    char usuario[100];
    char contrasena[100];
    Cliente cliente;
} Usuario;


typedef struct
{
    Cliente cliente;
    int numMesa;
    char Fecha [11];
    char Horario[6];
    int cantidadPersonas;
} Reserva;
void guardarReservas (Reserva *reservas, int numReservas);
int cargaReservas (Reserva **reservas);
int autenticacionUsuario(char *usuario,char *contrasena);
void creaReserva(Reserva **reservas,int *numReservas,Pila *pilon,Usuario usuarioAutenticado,int cantidadPersonasPorMesa);
void modificaReserva (Reserva *reservas, int numReservas, Pila *pilon,Usuario usuarioAutenticado,int cantidadPersonasPorMesa);
void bajaReseva (Reserva *reservas, int *numReservas, Pila *pilon, Usuario usuarioAutencicado,int cantidadPersonasPorMesa);
void  mostrarMesasDisponibles(Reserva *reservas,int numReserva);
void buscarReservas (Reserva *reservas, int numReservas);
int main()
{
    char soliReserva[] = {"Reservas.bin"};
    char Usuarios[] = {"usuarios.bin"};
    int cantidadPersonasPorMesa[MAX_MESAS];
    Reserva reservas;
    int numReservas = cargaReservas (&reservas);
    Pila pilaReservas;
    inicpila(&pilaReservas);


//inicializa el arreglo de cantidad de personas con 0
    for (int i=0; i<MAX_MESAS; i++)
    {
        cantidadPersonasPorMesa[i] =0;
    }
    char usuario[100],contrasena[100];
    Usuario usuarioAutenticado;
    printf("Ingrese su usuario: ");
    fflush(stdin);
    scanf("%s",&usuario);

    printf("Ingrese su contrasena: ");
    fflush(stdin);
    scanf("%s",&contrasena);

    if (!autenticacionUsuario (usuario,contrasena))
    {
        printf("Autenticacion fallida. Saliendo...\n");
        return 1;
    }
    int opcion;

    do
    {
        printf("\n Gestione su reserva");
        printf("1. Alta de Reserva\n");
        printf("2. Modificar Reserva\n");
        printf("3. Baja de Reserva\n");
        printf("4. Mostrar Mesas Disponibles\n");
        printf("5. Buscar Reservas\n");
        printf("6. Salir\n");
        printf("Seleccione una opcion: ");
        fflush(stdin);
        scanf("%i",&opcion);

        switch(opcion)
        {
        case 1:
            creaReserva(&reservas,&numReservas,&pilaReservas,usuarioAutenticado,cantidadPersonasPorMesa);
            break;
        case 2:
            modificaReserva(&reservas,&numReservas,&pilaReservas,usuarioAutenticado,cantidadPersonasPorMesa);
            break;
        case 3:
            bajaReseva (&reservas,&numReservas,&pilaReservas,usuarioAutenticado,cantidadPersonasPorMesa);
            break;
        case 4:
            mostrarMesasDisponibles (&reservas,numReservas);
            break;
        case 5:
            buscarReservas(&reservas,numReservas);
            break;
        case 6:
            printf("Saliendo...\n");
            break;
        default:
            printf("Opcion no valida.\n");
            break;
        }

    }
    while(opcion!= 6);
    free(&reservas);
    return 0;
}



void guardarReservas (Reserva *reservas, int numReservas)
{
    FILE *archi = fopen("Reservas.bin","ab");

    if (archi!=NULL)
    {
        fwrite(reservas,sizeof(Reserva),numReservas,archi);
        fclose(archi);
    }
    else
    {
        printf("Error al abrir el archivo para guardar las reservas.\n");
    }
}

int cargaReservas (Reserva **reservas)
{
    FILE *archi = fopen("Reservas.bin","rb");
    int NumReservas =0;
    if (archi!=NULL)
    {
        fseek (archi,0,SEEK_END);
        NumReservas = ftell(archi) / sizeof(Reserva);
        rewind (archi);
        reservas = (reservas *)malloc(NumReservas * sizeof(Reserva));
        fread (reservas,sizeof(Reserva),NumReservas,archi);
        fclose(archi);
    }
    return NumReservas;
}

int autenticacionUsuario(char *usuario,char *contrasena)
{
    FILE *archi = fopen("usuarios.bin","rb");
    Usuario u;
    int encontrado = 0;
    if (archi != NULL)
    {
        while (fread (&u,sizeof(Usuario),1,archi) == 1)
        {
            if (strcmp(u.usuario,usuario) == 0 && strcmp(u.contrasena,contrasena) == 0)
            {
                encontrado = 1;
                printf("Autenticacion exitosa.\n");
                printf("Datos del Cliente:\n");
                printf("Nombre: %s\n",u.cliente.Nombre);
                printf("Telefono: %s\n",u.cliente.Telefono);
                break;
            }
        }
        if (!encontrado)
        {
            printf("Usuario o Contraseña incorrecto.\n");
        }
    }
    return encontrado;
}

void creaReserva(Reserva **reservas,int *numReservas,Pila *pilon,Usuario usuarioAutenticado,int cantidadPersonasPorMesa)
{
    Reserva nuevaReserva;
    nuevaReserva.cliente = usuarioAutenticado.cliente;
    strcpy(nuevaReserva.cliente,usuarioAutenticado.usuario); // asocia reserva con usuario

    printf("Ingrese el numero de mesa (1-50: ");
    fflush(stdin);
    scanf("%i",&nuevaReserva.numMesa);
    printf("Ingrese la fecha (DD/MM/AAAA): ");
    fflush(stdin);
    scanf("&s",&nuevaReserva.Fecha);
    printf("Ingrese el horario (HH:MM): ");
    fflush(stdin);
    scanf("%s",&nuevaReserva.Horario);
    printf("Ingrese la cantidad de personas: ");
    fflush(stdin);
    scanf("%i",&nuevaReserva.cantidadPersonas);

    reservas = (Reserva *)realloc(reservas,(numReservas) * sizeof(Reserva));
    reservas[numReservas] = nuevaReserva;
    numReservas++;

    cantidadPersonasPorMesa[nuevaReserva.numMesa - 1] = nuevaReserva.cantidadPersonas;
    apilar(pilon,nuevaReserva.numMesa);

    guardarReservas(&reservas,&numReservas);

    printf("Reserva realizada con exito para el usuario %s.\n",usuarioAutenticado.usuario);
}

void modificaReserva (Reserva *reservas, int numReservas, Pila *pilon,Usuario usuarioAutenticado,int cantidadPersonasPorMesa)
{
    char fecha[11], horario[6];

    printf("Ingrese  la fecha de la reserva a modificar (DD/MM/AAAA): ");
    fflush(stdin);
    scanf("%i",&fecha);
    printf("Ingrese el horario de la reserva a modificar (HH/MM): ");
    fflush(stdin);
    scanf("%s",&horario);

    for (int i =0; i<numReservas; i++)
    {
        if (strcmp(reservas[i].cliente,usuarioAutenticado.usuario) == 0 && strcmp(reservas[i].Fecha,fecha) == 0 && strcmp(reservas[i].Horario,horario)== 0)
        {
            printf("Ingrese el nuevo numero de mesa (1-50): ");
            fflush(stdin);
            scanf("%i",&reservas[i].numMesa);
            printf("Ingrese la nueva cantidad de personas: ");
            fflush(stdin);
            scanf("%i",&reservas[i].cantidadPersonas);

            printf("Ingrese el nuevo horario(HH/MM): ");
            fflush(stdin);
            scan("%s",&reservas[i].Horario);

            printf("Ingrese la nueva fecha (DD/MM/AAAA): ");
            fflush(stdin);
            scanf("%s",&reservas[i].Fecha);

            cantidadPersonasPorMesa[reservas[i].numMesa - 1] = reservas[i].cantidadPersonas;

            guardarReservas(reservas,numReservas);

            printf("Reserva modificada con exito para el usuario %s.\n",usuarioAutenticado.usuario);
            return;
        }
    }
    printf("No se encontro una reserva para modificar.\n");
}

void bajaReseva (Reserva *reservas, int *numReservas, Pila *pilon, Usuario usuarioAutencicado,int cantidadPersonasPorMesa);
{
    char fecha[11],horario[6];

    printf("Ingrese la fecha de la reserva a eliminar (DD/MM/AAAA): ");
    fflush(stdin);
    scanf("%s",&fecha);

    printf("Ingrese el horatio de la reserva a eliminar (HH:MM): ");
    fflush(stdin);
    scanf("%s",&horario);

    for (int i =0; i<numReservas; i++)
    {
        if (strcmp(reservas[i].cliente,usuarioAutencicado.usuario) == 0 && strcmp (reservas[i].Fecha,fecha) == 0 && strcmp(strcmp(reservas[i].Horario,horario) == 0))
        {
            cantidadPersonasPorMesa[reservas[i].numMesa - 1] = 0;
            apilar(pilon,reservas[i].numMesa);

            for (int j = i; j<numReservas -1; j++)
            {
                reservas[j] = reservas[j +1];
            }
            numReservas--;
            reservas = (Reserva *)realloc(reservas),(numReservas * sizeof(Reserva));

            guardarReservas (reservas,&numReservas);

            printf("Reserva eliminada con exito para el usuario %s.\n",usuarioAutencicado.usuario);
        }
    }
    printf("No se encontro una reserva para eliminar.\n");
}
void  mostrarMesasDisponibles(Reserva *reservas,int numReserva)
{
    int horarios[21] = {0}; //cada posicion representa un  intervalo de 30 min desde las 12 hasta las 22
    char horas[21][6] =
    {
        "12:00", "12:30", "13:00", "13:30", "14:00", "14:30", "15:00", "15:30",
        "16:00", "16:30", "17:00", "17:30", "18:00", "18:30", "19:00", "19:30",
        "20:00", "20:30", "21:00", "21:30", "22:00"
    };
//marca mesas ocupadas en los horarios correspondientes
    for (int i =0; i<numReserva; i++)
    {
        for (int j =0; j<21; j++)
        {
            if (strcmp(reservas[i].Horario,horas[j]) == 0)
            {
                horarios[j]++;
            }
        }
    }
    for (int i = 0; i<21; i++)
    {
        printf("Horario %s: %d Mesas disponibles.\n",horas[i],MAX_MESAS - horarios[i]);
    }
}

void buscarReservas (Reserva *reservas, int numReservas)
{
    int opcion;
    char fecha[11];
    char horario[6];
    char nombre[100];
    int telefono[15];

    printf("Buscar por:\n");
    printf("1. Fecha\n");
    printf("2. Horario\n");
    printf("3. Nombre de Cliente\n");
    printf("4. Teléfono de Cliente\n");
    printf("Seleccione una opción: ");
    scanf("%d", &opcion);


    switch (opcion)
    {
    case 1:
        printf("Ingrese la fecha (DD/MM/AAAA): ");
        scanf("%s", &fecha);
        for (int i = 0; i < numReservas; i++)
        {
            if (strcmp(reservas[i].Fecha, fecha) == 0)
            {
                printf("Reserva encontrada: Mesa %d, Fecha %s, Horario %s, Cliente %s, Teléfono %s, Cantidad de personas: %d\n", reservas[i].numMesa, reservas[i].Fecha, reservas[i].Horario, reservas[i].cliente.Nombre, reservas[i].cliente.Telefono, reservas[i].cantidadPersonas);


            }
        }
        break;
    case 2:
        printf("Ingrese el horario (HH:MM): ");
        scanf("%s", &horario);
        for (int i = 0; i < numReservas; i++)
        {
            if (strcmp(reservas[i].Horario, horario) == 0)
            {
                printf("Reserva encontrada: Mesa %d, Fecha %s, Horario %s, Cliente %s, Teléfono %s, Cantidad de personas: %d\n",reservas[i].numMesa, reservas[i].Fecha, reservas[i].Horario, reservas[i].cliente.Nombre, reservas[i].cliente.Telefono, reservas[i].cantidadPersonas);


            }
        }
        break;
    case 3:
        printf("Ingrese el nombre del cliente: ");
        scanf("%s", &nombre);
        for (int i = 0; i < numReservas; i++)
        {
            if (strcmp(reservas[i].cliente.Nombre, nombre) == 0)
            {
                printf("Reserva encontrada: Mesa %d, Fecha %s, Horario %s, Cliente %s, Teléfono %s, Cantidad de personas: %d\n",  reservas[i].numMesa, reservas[i].Fecha, reservas[i].Horario, reservas[i].cliente.Nombre, reservas[i].cliente.Telefono, reservas[i].cantidadPersonas);


            }
        }
        break;
    case 4:
        printf("Ingrese el teléfono del cliente: ");
        scanf("%s", &telefono);
        for (int i = 0; i < numReservas; i++)
        {
            if (strcmp(reservas[i].cliente.Telefono, telefono) == 0)
            {
                printf("Reserva encontrada: Mesa %d, Fecha %s, Horario %s, Cliente %s, Teléfono %s, Cantidad de personas: %d\n", reservas[i].numMesa, reservas[i].Fecha, reservas[i].Horario,reservas[i].cliente.Nombre, reservas[i].cliente.Telefono, reservas[i].cantidadPersonas);


            }
        }
        break;
    default:
        printf("Opción no válida.\n");
        break;
    }
}




