#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>

sem_t mutex;

/*
int gameState
State of the game key:
0-preGame:
In this phase of the game players join the game and get dealer deals cards
to players in a round robin fashion. Once every player has 1 card, the dealer
will change gameState to mainGame(1)

1-mainGame:
In this phase the dealer will deal a card and then the player with id equal to
current turn will check their hand. If they have a match they declare victory
and change gameState to postGame(2). Else player places a random card at the
bottom of the deck and then the dealer will deal another card. This will repeat
until gameState is changed to postGame.

2-postGame:
In this phase the dealer will ask each player to place their hand at
the bottom of the deck. Each player will exit the round after they have placed
their hand at the bottom of the deck. Once all of the players have left the
dealer will start a new round by changing gameState to preGame(0).
*/

int gameState = 0;


/*
Theses next varibles are control varibles.
int currentTurn - whos turn it is.
int nextTurn - this is used by the dealer to keep track of which player is next.
int deltCard - int to store the card delt by the dealer.
int players - numbers of players in the game.
int deltCards - used in the preGame state in order so the dealer knows when
to change gameState to mainGame.
int rounds - this will keep track of the amount of rounds played.
int maxRounds - max number of rounds that will be played.


Key for who's turn it is
0-Dealer
1-Player 1
2-Player 2
3-Player 3
*/
int currentTurn = 0;
int goingFirst = 1;
int nextTurn = 1;
int deltCard;
int players = 0;
int deltCards = 0;
int rounds = 0;
int maxRounds = 3;


//Varibles for seed and the log file.
int seed = 0;
FILE * file;

/*
Attributes of the deck and declaration

int topDeck - This is to keep track of what the top card is.
int bottomDeck - This is to keep track of where the bottom of the deck is.
bottomDeck is where cards that are discarded will be placed.
*/

#define size 24
int deck[size];
int topDeck = 0;
int bottomDeck = 0;


//Individual Player hands.
int playerHand1[2];
int playerHand2[2];
int playerHand3[2];



/*
This will print the contents of the deck.
*/

void printDeck()
{
     int i = 0;
     int num;
     int deckIndex = topDeck;
     for(i; i < size; i++)
    {

        num = deck[deckIndex];
        if(num != 0)
        {
          printf("%d ", num);
          fprintf(file,"%d ", num);
        }

        deckIndex = (deckIndex + 1) % size;
    }
    printf("\n");
    fprintf(file,"\n");
}

/*
Basic Fisher-Yates shuffle. Used by the dealer to shuffle.
*/
void shuffleDeck()
{
    srand(seed);
    int i = size - 1;

    for(i; i > 0; i--)
    {
       int j = rand() % (i + 1);

       int temp = deck[i];
       deck[i] = deck[j];
       deck[j] = temp;

    }

}

/*
Thesse next functions are utilites for the player's to manage thier hands.

int checkHand(int id):
This function takes the player id as an argument.
It will check to see if the player has matching cards in their hand based
on the the player's id. It will return 0 if the player does have a match.
Otherwise will return 1.

void flushHand(int id):
This function take the player's id as an argument. Based on the player id
it will reset the player's hand and return all cards in their hand to the deck.

void drawCard(int id):
This function take the player's id as an argument. Based on the player id
it performs the draw operation whenever the player threads are in gameState = 1
and currentTurn = playerid.

void discardCard(int id):
This function take the player's id as an argument. Based on the player id
it will discard a random card from that player's hand. It reseeds
to determine which card to discard.
*/
int checkHand(int id)
{
  switch(id)
  {
    case 1:
    if(playerHand1[0] != playerHand1[1])
    {
        return 1;
    }
    return 0;
    break;
    case 2:
    if(playerHand2[0] != playerHand2[1])
    {
        return 1;
    }
    return 0;
    break;
    case 3:
    if(playerHand3[0] != playerHand3[1])
    {
        return 1;
    }
    return 0;
    break;

  }
}

void flushHand(int id)
{
  switch(id)
  {
    //player 1
    case 1:
    if(playerHand1[0] != 0)
    {
        deck[bottomDeck] = playerHand1[0];
        bottomDeck = (bottomDeck + 1) % size;
        playerHand1[0] = 0;
    }
    if(playerHand1[1] != 0)
    {
        deck[bottomDeck] = playerHand1[1];
        bottomDeck = (bottomDeck + 1) % size;
        playerHand1[1] = 0;
    }
    break;

    //player 2
    case 2:
    if(playerHand2[0] != 0)
    {
        deck[bottomDeck] = playerHand2[0];
        bottomDeck = (bottomDeck + 1) % size;
        playerHand2[0] = 0;
    }
    if(playerHand2[1] != 0)
    {
        deck[bottomDeck] = playerHand2[1];
        bottomDeck = (bottomDeck + 1) % size;
        playerHand2[1] = 0;
    }
    break;

    //player 3
    case 3:
    if(playerHand3[0] != 0)
    {
        deck[bottomDeck] = playerHand3[0];
        bottomDeck = (bottomDeck + 1) % size;
        playerHand3[0] = 0;
    }
    if(playerHand3[1] != 0)
    {
        deck[bottomDeck] = playerHand3[1];
        bottomDeck = (bottomDeck + 1) % size;
        playerHand3[1] = 0;
    }
    break;
  }
}

void drawCard(int id)
{

  //printf("Player %d: draws %d\n", id, deck[topDeck]);
  fprintf(file,"Player %d: draws %d\n", id, deck[topDeck]);
  /*
  printf("\n");
  printf("Top Deck: %d\n", topDeck);
  printf("BottomDeck: %d\n", bottomDeck);
  printf("\n");
  */

  deltCard = deck[topDeck];
  deck[topDeck] = 0;
  topDeck = (topDeck + 1) % size;




  switch(id)
  {
    case 1:
    if(playerHand1[0] != 0)
    {
        playerHand1[1] = deltCard;
        deltCard = 0;
    }
    else
    {
        playerHand1[0] = deltCard;
        deltCard = 0;
    }
    break;

    case 2:
    if(playerHand2[0] != 0)
    {
        playerHand2[1] = deltCard;
        deltCard = 0;
    }
    else
    {
        playerHand2[0] = deltCard;
        deltCard = 0;
    }
    break;

    case 3:
    if(playerHand3[0] != 0)
    {
        playerHand3[1] = deltCard;
        deltCard = 0;
    }
    else
    {
        playerHand3[0] = deltCard;
        deltCard = 0;
    }
    break;
  }
}

void discardCard(int id)
{
  srand(seed);
  int discard = rand() % 2;

  switch(id)
  {
    case 1:
    //printf("Player %d: discards %d\n", id, playerHand1[discard]);
    fprintf(file,"Player %d: discards %d\n", id, playerHand1[discard]);
    deck[bottomDeck] = playerHand1[discard];
    bottomDeck = (bottomDeck + 1) % size;
    playerHand1[discard] = 0;
    break;

    case 2:
    //printf("Player %d: discards %d\n", id, playerHand2[discard]);
    fprintf(file,"Player %d: discards %d\n", id, playerHand2[discard]);
    deck[bottomDeck] = playerHand2[discard];
    bottomDeck = (bottomDeck + 1) % size;
    playerHand2[discard] = 0;
    break;

    case 3:
    //printf("Player %d: discards %d\n", id, playerHand3[discard]);
    fprintf(file,"Player %d: discards %d\n", id, playerHand3[discard]);
    deck[bottomDeck] = playerHand3[discard];
    bottomDeck = (bottomDeck + 1) % size;
    playerHand3[discard] = 0;
    break;
  }
}

/*
These functions are the logic for the player and dealer.
They are both Thread functions.
*/
void dealerLogic()
{
   while(rounds < maxRounds)
   {

   if(currentTurn != 0)
   {
       //printf("Dealer is wating.\n);
   }
   else
   {
       switch(gameState)
       {
          case 0:
          if(deltCards == 3)
          {
              deltCards = 0;
              printf("Let the game begin player %d is first.\n", nextTurn);
              fprintf(file,"Let the game begin player %d is first.\n", nextTurn);
              fprintf(file,"Deck contains: ");
              printf("Deck contains: ");
              printDeck();
              currentTurn = nextTurn;
              nextTurn = (nextTurn % 3) + 1;
              gameState = 1;

          }
          else
          {
             //printf("Dealer deals a card to player %d\n", nextTurn);
             sem_wait(&mutex);
             deltCard = deck[topDeck];
             deck[topDeck] = 0;
             topDeck = (topDeck + 1) % size;
             deltCards += 1;
             //printf("Deck contains: ");
             //printDeck();
             currentTurn = nextTurn;
             nextTurn = (nextTurn % 3) + 1;
             sem_post(&mutex);
          }
          break;

          case 1:

          //printf("Dealer deals a card to player %d\n", nextTurn);
          sem_wait(&mutex);
          currentTurn = nextTurn;
          nextTurn = (nextTurn % 3) + 1;
          sem_post(&mutex);
          break;

          case 2:
          if(players == 0)
          {
              rounds += 1;
              if(rounds < maxRounds)
              {
              printf("\nTime for round %d\n", rounds + 1);
              fprintf(file,"\nTime for round %d\n", rounds + 1);
              sem_wait(&mutex);
              topDeck = 0;
              bottomDeck = topDeck;
              printf("Dealer: deck shuffled.\n");
              fprintf(file,"Dealer: deck shuffled.\n");
              shuffleDeck();
              printf("Deck contains: ");
              printDeck();
              nextTurn = (goingFirst % 3) + 1;
              goingFirst = (goingFirst % 3) + 1;
              gameState = 0;
              sem_post(&mutex);
              }
              else
              {
                  printf("Dealer: The game is over.\n");
                  fprintf(file,"Dealer: The game is over.\n");
                  pthread_exit(0);
              }

          }
          else
          {
              //printf("Dealer asks player %d to flush their hand.\n", nextTurn);
              sem_wait(&mutex);
              //printf("Top Deck: %d\n", topDeck);
              //printf("Bottom deck: %d\n", bottomDeck);
              currentTurn = nextTurn;
              nextTurn = (nextTurn % 3) + 1;
              sem_post(&mutex);
          }
          break;

       }
   }
   }
   pthread_exit(0);
}


void playerLogic(void *ptr)
{
   int playerid = (int *)ptr;

   while(rounds < maxRounds)
   {

   if(currentTurn != playerid)
   {
       //printf("Player %d is waiting.\n", playerid);
   }
   else
    {
       switch(gameState)
       {
          case 0:
          sem_wait(&mutex);

          //printf("Player %d added the delt card to their hand.\n", playerid);
          if(playerid == 1)
          {
            playerHand1[0] = deltCard;
            deltCard = 0;
          }
          if(playerid == 2)
          {
            playerHand2[0] = deltCard;
            deltCard = 0;
          }
          if(playerid == 3)
          {
             playerHand3[0] = deltCard;
             deltCard = 0;
          }

          players += 1;
          currentTurn = 0;
          sem_post(&mutex);
          break;

          case 1:
          drawCard(playerid);
          //printf("Deck contains: ");
          //printDeck();

          if(playerid == 1)
          {
            printf("Player 1 Hand: %d %d \n", playerHand1[0], playerHand1[1]);
            fprintf(file,"Player 1 Hand: %d %d \n", playerHand1[0], playerHand1[1]);
          }
          if(playerid == 2)
          {
             printf("Player 2 Hand: %d %d \n", playerHand2[0], playerHand2[1]);
             fprintf(file,"Player 2 Hand: %d %d \n", playerHand2[0], playerHand2[1]);
          }
          if(playerid == 3)
          {
             printf("Player 3 Hand: %d %d \n", playerHand3[0], playerHand3[1]);
            fprintf(file,"Player 3 Hand: %d %d \n", playerHand3[0], playerHand3[1]);
          }
          if(checkHand(playerid) != 0)
          {
              sem_wait(&mutex);
              //printf("Player %d does not have a matching card.\n", playerid);
              discardCard(playerid);
              if(playerid == 1)
             {
                if(playerHand1[0] == 0)
                {
                  //printf("Player %d Hand: %d \n", playerid, playerHand1[1]);
                  fprintf(file,"Player %d Hand: %d \n", playerid, playerHand1[1]);
                }
                else
                {
                    //printf("Player %d Hand: %d \n", playerid, playerHand1[0]);
                    fprintf(file,"Player %d Hand: %d \n", playerid, playerHand1[0]);
                }

             }
             if(playerid == 2)
             {
                if(playerHand2[0] == 0)
                {
                  //printf("Player %d Hand: %d \n", playerid, playerHand2[1]);
                 fprintf(file,"Player %d Hand: %d \n", playerid, playerHand2[1]);
                }
                else
                {
                    //printf("Player %d Hand: %d \n", playerid, playerHand2[0]);
                    fprintf(file,"Player %d Hand: %d \n", playerid, playerHand2[0]);
                }
             }
             if(playerid == 3)
             {
               if(playerHand3[0] == 0)
                {
                  //printf("Player %d Hand: %d \n", playerid, playerHand3[1]);
                  fprintf(file,"Player %d Hand: %d \n", playerid, playerHand3[1]);
                }
                else
                {
                    //printf("Player %d Hand: %d \n", playerid, playerHand3[0]);
                    fprintf(file,"Player %d Hand: %d \n", playerid, playerHand3[0]);
                }
             }
              printf("Deck contains: ");
              fprintf(file,"Deck contains: ");
              printDeck();
              printf("Player %d did not win this round.\n", playerid);
              currentTurn = nextTurn;
              nextTurn = nextTurn % 3 + 1;
              sem_post(&mutex);
          }
          else
          {
              //Declare victory
              printf("Player %d has won this round!\n", playerid);
              fprintf(file,"Player %d has won this round!\n", playerid);
              printf("Deck contains: ");
              printDeck();
              currentTurn = 0;
              gameState = 2;
          }
          break;

          case 2:
          //Flush hand
          sem_wait(&mutex);
          flushHand(playerid);
          //printf("Player %d returns hand to the deck.\n", playerid);
          //printf("Player %d exits the round.\n", playerid);
          fprintf(file,"Player %d exits the round.\n", playerid);
          //printf("Deck contains: ");
          //printDeck();
          players--;
          currentTurn = 0;
          sem_post(&mutex);
       }
    }
   }
   pthread_exit(0);
}


int main(int argc, char *argv[])
{
    int argSeed = 4;
    //seed = atoi(argv[1]);
    seed = argSeed;
    //Create the deck.
    int nextNumber = 1;
    int i = 0;
    for(i; i < size; i++)
    {
        deck[i] = nextNumber;
        nextNumber = (nextNumber % 6) + 1;
    }

    //Log file
   file = fopen("pair_war.log", "a+");
   fprintf(file, "----New Iteration----");

    shuffleDeck();
    printf("Deck Contains: ");
    printDeck();


    pthread_t dealer;
    pthread_t player1;
    pthread_t player2;
    pthread_t player3;

    int p1ID = 1;
    int p2ID = 2;
    int p3ID = 3;

    pthread_create(&dealer, NULL, (void *) &dealerLogic, NULL);
    pthread_create(&player1, NULL, (void *) &playerLogic, (void *)p1ID);
    pthread_create(&player2, NULL, (void *) &playerLogic, (void *)p2ID);
    pthread_create(&player3, NULL, (void *) &playerLogic, (void *)p3ID);

    pthread_join(dealer, NULL);
    pthread_join(player1, NULL);
    pthread_join(player2, NULL);
    pthread_join(player3, NULL);

    fclose(file);


    return 0;
}
