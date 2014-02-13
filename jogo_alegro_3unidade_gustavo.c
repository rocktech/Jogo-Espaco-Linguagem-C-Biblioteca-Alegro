
#define ORIGEM	    0    //cordenadas do  jogo
#define LIFE_LIMIT  3    // 
#define FADE_VEL    2    // faz a tela surgir e desaparecer ..causando  a sensacao  massa 
#define SCORE_TOTAL 200   
#define NAVE_X	    70   // dimensao  da nave
#define NAVE_Y      47
#define TIRO_X	    14   // dimensao  do  tiro
#define TIRO_Y      3
#define TIRO_TX     12   
#define EXPLOSAO_X  40   // dimencao  da explosao
#define EXPLOSAO_Y  40
#define AST_X	    54   // dimencao dos asteroides
#define AST_Y       54
#define MAX_X       800  
#define MAX_Y       600 
#define COLOR_BITS	16   
#define true	    1
#define false	    0
#define PAUSE_X     320  
#define PAUSE_Y     102  
#define MOV_NAVE	3    
#define MOV_TIRO	4     
#define MOV_AST0    4    
#define MOV_AST1    2   

//as bibliotecas usadas no meu  jogo
// eu  suzyanne quero  mostrar para marcos sobre o  github

#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


//estruturas ----achei  melhor assim .....fica mais facil  de manipular o  meu  jogo  
typedef int bool;

typedef struct{ SAMPLE *smenu;
                BITMAP *tmenu;
                } MENU_PR;

typedef struct{ int posx,posy,value;
                BITMAP *marcador;
                } MARCADOR;

typedef struct{ int x,y,life;
                BITMAP *imagem;
                MIDI *endlife;
                } NAVE;

typedef struct{ int c_asteroide;
                BITMAP *img_ast;
                } ASTEROIDE;

typedef struct{ int c_tiro;
                BITMAP *img_tiro;
                SAMPLE *somtiro;
                } TIRO;
                
typedef struct{ BITMAP *colisao;
                SAMPLE *explosao;
                } COLISAO;

struct NO{ int px,py,item;
           struct NO *prox;
           };

typedef struct{ struct NO *inicio;
                struct NO *fim;
                struct NO *corrente;
                } IMG;

//----------------------FUNCOES DO  JOGO----------------------------------------------

//funcao  que incializa o  allegro
void init() {
	int depth;
	allegro_init();
	depth = desktop_color_depth();
	
    if (depth == 0) depth = COLOR_BITS;
	set_color_depth(depth);
	set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, MAX_X, MAX_Y, 0, 0);
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
	install_timer();
	install_keyboard();//mouse
	srand(time(NULL));
   
	
}
void deinit() {//funcao  que  finaliza o  allegro 
	clear_keybuf();
	allegro_exit();
}
void deinit_imagem( BITMAP *aux1, BITMAP *aux2, BITMAP *aux3, BITMAP *aux4, BITMAP *aux5 ) {
//funcao  que  destroi  os bitmmp criados 
   destroy_bitmap(aux1);
   destroy_bitmap(aux2);
   destroy_bitmap(aux3);
   destroy_bitmap(aux4);
   destroy_bitmap(aux5);  
}
void fundo_tela( BITMAP *bg ){//desenha o  plano  de funo  do  game  "campo estrelar"
     int i;

     for(i=0;i<200;i++){
        if(i%4==0) circlefill(bg, rand()%MAX_X, rand()%(MAX_Y), rand()%1, makecol(255,255,255) );
        if(i%5==0) circlefill(bg, rand()%MAX_X, rand()%(MAX_Y), rand()%1, makecol(0,0,255) );
     }

}
void texto_atualizado( BITMAP *buf, int *nl, int *s){//mostra os dados quando  atualizados na tela ...pontuacao  e vidas 
     
     if( (*nl) > 0 )
         textprintf_ex( buf, font, 10, 20, makecol(255,0,0), -1, "Life  : 0%d", (*nl) );
     else
         textprintf_ex( buf, font, 10, 20, makecol(255,0,0), -1, "Life  : 0%d", 0 );
     
     if( (*s) > 9 )
         textprintf_ex( buf, font, 10, 30, makecol(255,0,0), -1, "Score : %d" , (*s)  );
     else
         textprintf_ex( buf, font, 10, 30, makecol(255,0,0), -1, "Score : 0%d", (*s)  );
}
//---movimentos da minha nave espacial-------
int movimento_geral( int *m, int *n ){
     //vai  incrementar os contadores controlando  assim os asteriodes que estao  sendo  lancados 
     (*n)++;
     if( !key[KEY_LCONTROL] ) {(*m)++; return 0;}
     else {(*m)+=3; return 3;}
     
}
void movimenta_nave(BITMAP *buf, NAVE *nave ){//funcao  responsavel  pela movimentacao  da nave 
//como  a parte das teclas  com  a movimentacao  de acordo  com  o  teclado 

      if ( key[KEY_UP] )//movimenta a nave para cima ,quando  clicado  seta para cima 
      {
         if( (nave->y > ORIGEM) )         nave->y -= MOV_NAVE;
      }
      if ( key[KEY_DOWN] )//movimenta a nave para baixo ,quando  clicado  seta para baixo 
      {
         if( (nave->y < MAX_Y - NAVE_Y) ) nave->y += MOV_NAVE;
      }
      if ( key[KEY_LEFT] )//movimenta a nave para a esquerda ,quando  clicado  seta paa esquerda 
      {
         if( (nave->x > ORIGEM) )         nave->x -= MOV_NAVE;
      }
      if ( key[KEY_RIGHT] )//movimenta a nave para a direita  ,quando  clicado  seta para direita 
      {
         if( (nave->x < MAX_X - NAVE_X) ) nave->x += MOV_NAVE;
      }
      
      draw_sprite(buf,nave->imagem,(nave->x),(nave->y)); //com tudo  vai  atualizando  a posicao  da nave no  buffer

}
void movimenta_fundo_tela(BITMAP *bg, BITMAP *buf, int *i){
     //essa funcao  e responsavel  pela impressao  de movimento  e faz com que  nao  fique  
     //tremendo  a imagem quando  movimentado .        
        blit(bg    , buf, (*i)        ,  0,  0,  0, MAX_X, MAX_Y);
        blit(bg    , buf, (*i) - MAX_X,  0,  0,  0, MAX_X, MAX_Y);
        if( (*i) >= MAX_X ) (*i) = 0;

}
//funcoes que usam lista encadeada...no qual  eu  teve que estudar muito  ...para colocar no  jogo  e ficar melhor 
void init_IMG(IMG *l){//inicilizacao  na lista 
     l->inicio   = NULL;
     l->fim      = NULL;
     l->corrente = NULL;
}
bool adiciona_lista(IMG *l, int x, int y, int z){//funcao  que sao  adicionadas na minha lista 
	struct NO *aux;
	aux = (struct NO *)malloc(sizeof(struct NO));
	
	if( aux == NULL ){
		return false;
	}else{
          
		aux->item = z;		
				
		if( z % 2 == 0 ) {
             aux->px     = x + NAVE_X;	
		     aux->py     = y + ( (NAVE_Y - TIRO_Y)/2 );
        }
		else{
             aux->px     = x;	
		     aux->py     = y;
        }
		
        if( l->inicio == NULL ){
            //adiciona o  primeiro  elemento  na lista 
			  l->corrente = aux;
              l->inicio   = aux;
              l->fim      = aux;			
		}else{
			  (l->fim)->prox = aux; 
			  //ja aqui  vai  ser adicionado  no  final
			  l->fim = (l->fim)->prox;
		}
		l->fim->prox = NULL;
	} 
	
	return true;
}
bool rem(IMG *l,int v){//remove na lista 
	struct NO *ant  =  NULL;
	
	l->corrente = l->inicio;
	
	  while( l->corrente != NULL && l->corrente->item != v ){
	  	  ant         = l->corrente;
		  l->corrente = l->corrente->prox;
	  }
	
	  if( l->corrente == NULL ) 
      
          return false;
		
	  if( ant == NULL ){//se escolhido  o  primeiro da lista
		  l->inicio = l->corrente->prox;
		  l->corrente->prox = NULL;
		  free(l->corrente);
		  return true;
	  }
      
      if( l->corrente->prox == NULL ){//se escolhido  o ultimo da lista
		  l->fim = ant;
		  l->fim->prox = NULL;
		  free(l->corrente);
		  return true;
	  }
      
      ant->prox = l->corrente->prox;//qualquer um da lista 
	  l->corrente->prox = NULL;
	  free(l->corrente);
	  
	  return true;	
}
void movimenta_tiro( BITMAP *buf, BITMAP *tiro, IMG *t ){//funcao  responsavel  pela  movimentacao  dos tiros na tela 
     t->corrente = t->inicio;
     
     while( t->corrente != NULL ){
            
            t->corrente->px += MOV_TIRO;
            //aqui  vai  ser atualizado  ou  removido caso  o asteroide nao seja acertado e depois  tudo  isso  e quardado  no  buffer
            if( t->corrente->px < (MAX_X + TIRO_X) ){
                  draw_sprite( buf, tiro, t->corrente->px, t->corrente->py);
            }else{
                  rem( t , (t->corrente->item) );
            }
            
     t->corrente = t->corrente->prox;
     }

}
void lanca_tiro( BITMAP *buf, TIRO *tir, IMG *t, int *i, int x, int y){
      //dispara o  tiro adicionando  ele a lista 
      if( (key[KEY_SPACE]) && ((*i) % TIRO_TX == 0) ) {
          tir->c_tiro += 2;
          if(  adiciona_lista(t,x,y,(tir->c_tiro))  ){
          draw_sprite( buf, tir->img_tiro, (t->fim->px), (t->fim->py) );
          play_sample( tir->somtiro, 255, 128, 1000, false);//som  do  tiro
          
          }
          
      }

}
void movimenta_asteroide( BITMAP *buf, ASTEROIDE *ast, IMG *a, int acel){
     int TM;
     //funcao  que vai  movimentar os asteroides tanto horizantal  como  para cima ou para baixo  
     a->corrente = a->inicio;
    while( a->corrente != NULL ){
       
       TM = acel;
       if( (a->corrente->item) % 3 == 0 ){TM += MOV_AST1;}
       else{TM += MOV_AST0; }
     
          a->corrente->px -= TM;            
          
          if((a->corrente->item) % 5 == 0){  
             a->corrente->px -= TM;
             a->corrente->py += 2;
          }
          if((a->corrente->item) % 3 == 0){  
             a->corrente->px -= 1;
             a->corrente->py -= 1;
          }
          //atualiza ou  remove a posicao  do  asteroide no buffer
            if( (a->corrente->px > (ORIGEM - AST_X)) || (a->corrente->py > (ORIGEM - AST_Y)) || (a->corrente->py > (MAX_Y + AST_Y)) ){
                  draw_sprite( buf, ast->img_ast, a->corrente->px, a->corrente->py);
            }else{
                  rem( a , (a->corrente->item) );
            }
            
     a->corrente = a->corrente->prox;     
     }

}
void lanca_asteroide( BITMAP *buf, ASTEROIDE *ast, IMG *a, int i, int *j){
     int decide;
   //lanca cada asteroide de uma vez 
     if( (*j) == 3000 ) (*j) = 0;
   
         if( (*j) <  1000 ) 
             decide = 30;
         else 
             decide = 10;
     
     
     if( i % decide == 0 ){
         ast->c_asteroide += 2;
            adiciona_lista( a, MAX_X, (rand()%(MAX_Y - AST_Y)), (ast->c_asteroide) );
            draw_sprite(  buf, ast->img_ast, (a->fim->px), (a->fim->py) );
     }

}
void efeito_tela_escura(BITMAP *bmpOriginal, float vel ){//escurece a tela 
    if( vel > 0 ) {
        
        BITMAP *bmpArmazenador = NULL;
	    int var = 0;
        
        vel *= FADE_VEL;
    
		if( ( bmpArmazenador = create_bitmap( MAX_X, MAX_Y ) ) ) 
		{				
			for(var = 255 - FADE_VEL; var > 0; var -= vel) 
   			{
				clear(bmpArmazenador);
				set_trans_blender(0, 0, 0, var);
				draw_trans_sprite(bmpArmazenador, bmpOriginal, 0, 0 );
				vsync();
				blit(bmpArmazenador, screen, 0, 0, 0, 0, MAX_X, MAX_Y );
			}
			destroy_bitmap(bmpArmazenador);
		}
		
	
    } 
	
}
void efeito_tela_clara(BITMAP *bmpOriginal, float vel ){//clarear a tela 
    
    if( vel > 0 ) {
        BITMAP *bmpArmazenador = NULL;
	    int var = 0;
        vel *= FADE_VEL;
    
	if( ( bmpArmazenador = create_bitmap( MAX_X, MAX_Y ) ) )
	{
		
		for(var = 0; var < 256; var += vel) 
  		{
			clear(bmpArmazenador);
			set_trans_blender(0, 0, 0, var);
			draw_trans_sprite(bmpArmazenador, bmpOriginal, 0, 0);
			vsync();
			blit( bmpArmazenador, screen, 0, 0, 0, 0, MAX_X, MAX_Y );
		}
		destroy_bitmap(bmpArmazenador);
	}
	blit( bmpOriginal, screen, 0, 0, 0, 0, MAX_X, MAX_Y );
	
    } 
    
}

int simet(int x){ return ( (-1) * x ); }
//as colisoes do  jogo--------------------------------------
void perder_vida_jogo( NAVE *n ){
     //vai  ver se e a ultima vida que  o  usuario  do  jogo  tem ...pois  apartir dai  vai  
     //surgir o  som  que  a nave vai  explodir 
     if( n->life == 0 ){
         SAMPLE *al;
         stop_midi();
         al = load_wav("sound/alarme.wav");//som  do  alarme ..informando  que  vai  explodir na proxima colisao
         play_sample( al, 255, 128, 1000, false);
         play_looped_midi( n->endlife, false, simet(true) );
         
     }
     
}
void colisao2_nave( BITMAP *buf, COLISAO *n, IMG *a, NAVE *nave ){
     //se hover colisao vai  ser atualizado  no  buffer por essa funcao     
     draw_sprite( buf, n->colisao, (nave->x + 10 + (NAVE_X - EXPLOSAO_X)/2), (nave->y + ((NAVE_Y - EXPLOSAO_Y)/2) ) );
     rem(a,a->corrente->item);
     play_sample( n->explosao, 150, 128, 1000, false);
     (nave->life)--;
     perder_vida_jogo(nave);
}
void colisao2_tiro( BITMAP *buf, COLISAO *tiro, IMG *a, IMG *t, int *w ){
     draw_sprite( buf, tiro->colisao, (a->corrente->px - (AST_X/2)), (a->corrente->py + ((AST_Y - EXPLOSAO_Y)/2) ) );
     rem(a,a->corrente->item);
     rem(t,t->corrente->item);
     play_sample( tiro->explosao, 150, 128, 1000, false);
     (*w)++;
}
void colisao_tiro( BITMAP *buf, COLISAO *tiro, IMG *a, IMG *t, int *w ){//colisao  dos asteroides com os tiros 
     int x,y;
     a->corrente = a->inicio;
     while(a->corrente != NULL){
          t->corrente = t->inicio;
          while(t->corrente != NULL){
                      x = (a->corrente->px + (AST_X/2)) - ( t->corrente->px + TIRO_X );
                     
                     if( x < 0 ) x = ( t->corrente->px + TIRO_X ) - (a->corrente->px + (AST_X/2));
                     
                     if( x <= (AST_X/2) + (MOV_TIRO) ){
                          
                          y = (a->corrente->py + (AST_Y/2)) - (t->corrente->py + (TIRO_Y/2));
                          
                          if( y < 0 ) y = (t->corrente->py + (TIRO_Y/2)) - (a->corrente->py + (AST_Y/2));
                     
                      
                       if( hypot(x,y) <= ( (AST_X+AST_Y)/4 ) ){//calcula a ditancia entre o  tiro  e o  asteroide 
                       //para dizer que  o asteroide foi  acertado
                         colisao2_tiro( buf, tiro, a, t, w );
                         break;
                       }
                     }          
          t->corrente = t->corrente->prox;
          }
     a->corrente = a->corrente->prox;
     }
     
}
void colisao_nave( BITMAP *buf, COLISAO *n, IMG *a, NAVE *nave ){//colisao  asteroide  x  nave
     int x1,NX1,y,x2,NY2;
     a->corrente = a->inicio;
     while(a->corrente != NULL){
        NX1 = 40;
        NY2 = 8;
        
        x1 = (a->corrente->px + (AST_X/2)) - ( nave->x + NX1 );
        x2 = (a->corrente->px + (AST_X/2)) - ( nave->x + (NAVE_X - (NY2/2) ) );
        
        
        if( (x1 <= (AST_X + NAVE_Y)/2) + 6 || (x2 <= (AST_X + NY2)/2) + 6 ){ 
                                                                             
            y = (nave->y + (NAVE_Y/2)) - (a->corrente->py + (AST_Y/2));
            if( y1 < 0 ) y = (a->corrente->py + (AST_Y/2)) - (nave->y + (NAVE_Y/2));
                     
                  if( ( y <= (AST_Y + NAVE_Y)/2 ) || ( y <= (AST_Y + NY2)/2 ) ){
     
                    if( ((hypot(x1,y) < ((AST_X + NX1)/2 ) + 2)) || (hypot(x2,y) < ( (AST_Y + NY2)/2 ) ) ){//calcula 
                      colisao2_nave( buf, n, a, nave );
                      break;
                    }     
                  }              
        }                         
     a->corrente = a->corrente->prox;
     }
}
//funcao  que  eu  fiz de ultima hora ...tive essa ideia de dar o  pause no  jogo  
void pause( BITMAP *buf ){

    if( key[KEY_P] ){//butao  P
        
        BITMAP *TEMP;
        TEMP = load_bitmap( "img/pause.bmp", NULL );//a imagem  pause quando  o  usuario clica P
        efeito_tela_escura( 5, 0.0 );
        blit( TEMP, screen, 2, 2, ((MAX_X - PAUSE_X)/4), ((MAX_Y - PAUSE_Y)/4), MAX_X, MAX_Y );
        while( !key[KEY_ENTER] );//so  sai  do  pause quando  o  usuario  clica enter
        efeito_tela_clara(5, 0.0 );
        destroy_bitmap(TEMP);
    }

}
//menu  principal-------------------------------------menu  do  meu  jogo
void move_marcador( MARCADOR *m, int i ){//movimetacao do  marcador "aviazinho " possiveis 
      //movimentacoes para cima e para baixo  ate ser clicado  enter 
      if ( key[KEY_UP] && (i%4 == 0) )
      {
         if( m->value != 0 ) {m->posy -= 50; m->value--; }
         
         else {m->posy = 540; m->value = 3;}
      }
      if ( key[KEY_DOWN] && (i%4 == 0) )
      {
         if( m->value != 3 ) {m->posy += 50; m->value++; }
         
         else {m->posy = 390; m->value = 0;}
      }
}
BITMAP *menu(BITMAP *tela){//funcoes  do menu 
     BITMAP *menu,*buf;
     menu = load_bitmap( "img/menu.bmp", NULL );//imagem  menu 
     buf  = create_bitmap( MAX_X, MAX_Y );
     draw_sprite( buf, tela, 0, 0 );
     draw_sprite( buf, menu, 0, 0 );
     destroy_bitmap(menu);
     return buf;
}
void creditos_jogo( ){//creditos 
     BITMAP *bg;
     bg = load_bitmap("img/creditos.bmp", NULL);
     draw_sprite( screen, bg, 0, 0 );
     while(!key[KEY_ESC]);
     destroy_bitmap(bg);
}
void instrucoes( ){//instrucoes 
     BITMAP *bg;
     bg = load_bitmap("img/instrucoes.bmp", NULL);
     draw_sprite( screen, bg, 0, 0 );
     while(!key[KEY_ESC]);
     destroy_bitmap(bg);
}
void final_game(){//imagem  que aparece se vencer o  jogo
     BITMAP *bg;
     bg = load_bitmap("img/venceu.bmp", NULL);
     draw_sprite( screen, bg, 0, 0 );
     while(!key[KEY_ESC]);
     destroy_bitmap(bg);
}

void game_over(){
     BITMAP *bg;
     bg = load_bitmap("img/game_over.bmp", NULL);//imagem  game  over 
     draw_sprite( screen, bg, 0, 0 );
     while(!key[KEY_ESC]);//sai quando  clica esc
     destroy_bitmap(bg);
}
// INICIA O JOGO;
void play_game(BITMAP *tela){

   BITMAP *double_buffer;
   MIDI *trilha;
   int i,j,score,acelera;
   
   TIRO tiros;
   ASTEROIDE ast;
   NAVE nave;
   COLISAO nt;
   IMG tiro, asteroide;
             //variaves do jogo
            j = 0;
            acelera = 0;
            score = 0;
            tiros.c_tiro = 0;
            ast.c_asteroide = -1;
             //listas 
            init_IMG(&tiro);
            init_IMG(&asteroide);

         tiros.img_tiro = load_bitmap( "img/tiro.bmp",       NULL );
         ast.img_ast    = load_bitmap( "img/asteroide.bmp",  NULL );
         nt.colisao     = load_bitmap( "img/explosao.bmp",   NULL );
         double_buffer  = create_bitmap( MAX_X, MAX_Y );
         nave.imagem = load_bitmap( "img/nave.bmp", NULL );
         nave.life   = LIFE_LIMIT;
         nave.x      = MAX_X/10;
         nave.y      = MAX_Y/2;
                      
         trilha       = load_midi("sound/metal2.mid");
         nave.endlife = load_midi("sound/metal3.mid");
         tiros.somtiro = load_wav("sound/tiro.wav");
         nt.explosao = load_wav("sound/explosao.wav");

   play_looped_midi( trilha, false, simet(true) );//exucuta o  som  da trilha 
   i = ORIGEM; //movimento do  game
   draw_sprite( double_buffer, tela, 0, 0 );
   draw_sprite( double_buffer, nave.imagem, nave.x, nave.y );
   efeito_tela_clara(  double_buffer, 1.5 );
 //percebi  logo  no  inicio  que iria precisava  desse laco  ...estai  o laco  principal  do  jogo
   while( !key[KEY_ESC] && (nave.life >= 0) && (score < SCORE_TOTAL) )
   {
      clear( double_buffer );
      movimenta_fundo_tela( tela, double_buffer, &i );
      movimenta_nave( double_buffer, &nave );
      lanca_asteroide( double_buffer, &ast, &asteroide, i, &j );
      movimenta_asteroide( double_buffer, &ast, &asteroide, acelera);
      lanca_tiro( double_buffer, &tiros, &tiro, &i, nave.x, nave.y );
      movimenta_tiro( double_buffer, tiros.img_tiro, &tiro );
      colisao_nave( double_buffer, &nt, &asteroide, &nave );
      colisao_tiro( double_buffer, &nt, &asteroide, &tiro, &score );
      pause( double_buffer );//caso  um pause durante o  jogo
      texto_atualizado(double_buffer,&nave.life,&score);
      vsync();
      blit( double_buffer, screen, 0, 0, 0, 0, MAX_X, MAX_Y );
      acelera = movimento_geral(&i,&j);//movimentacao  do  senario  do  jogo    
      
   }
   efeito_tela_escura( double_buffer, 1.5 );
   if ( score == SCORE_TOTAL ) 
      final_game();
   if ( nave.life < 0 )        
      game_over();
      stop_midi();
      deinit_imagem( ast.img_ast, double_buffer, nave.imagem, tiros.img_tiro, nt.colisao );

}

//--------------MAIN--------------------------------------------------------------
//minha funcao  principal main 
int main(){
    
   // inicio  do  game no  allegro
   init();
   
   MIDI *menu2;
   BITMAP *tela,*buff;
   int i;
   
   MENU_PR primeiro;
   MARCADOR marc;//marcador do  menu o  aviazinho da escolha da opcao
   bool sair;
      sair = false;
      
      marc.marcador = load_bitmap( "img/marcador.bmp", NULL );//nave que fica mexendo no  menu
      marc.posx  = 220;//posicao  da navezinha do  menu x 
      marc.posy  = 390;//posicao y
      marc.value = 0;
      
      menu2 = load_midi("sound/mus_menu.mid");//som inicial  do  menu
      //formacao  da imagem na tela 
      tela = create_bitmap( MAX_X, MAX_Y );
      fundo_tela(tela);
      buff = create_bitmap( MAX_X, MAX_Y );
      primeiro.tmenu = menu(tela);
   
   play_looped_midi( menu2, false, simet(true) );
   efeito_tela_clara(primeiro.tmenu,1.5);
   
   i=0;
   while( sair != true ){
   clear(buff);
   draw_sprite( buff, primeiro.tmenu, 0, 0 );
   move_marcador(&marc,i);
   
   if ( key[KEY_ENTER] ){//menu  ....
   
      switch(marc.value){
            case 0: stop_midi();//chamada da funcao  para jogar   
                    play_game(tela);
                    play_looped_midi( menu2, false, simet(true) );
                    break;
                    
            case 1: instrucoes();//chamada da funcao instrucoes 
                    break;
                    
            case 2: creditos_jogo();//chamda da funcao  creditos 
                    break;
                    
            case 3: sair = true;//para sair do  jogo  
                    break;
      }
   
   }
   
   draw_sprite( buff, marc.marcador, marc.posx, marc.posy );
   vsync();
   draw_sprite( screen, buff, 0, 0 );
   while( !keypressed() );
   i++;
   }
   efeito_tela_escura(buff,1.5);
   stop_midi();
   deinit_imagem( tela, primeiro.tmenu, marc.marcador, buff, NULL );
   deinit();
   return 0;
}END_OF_MAIN();//terminado  no  alegro 





