
;   main.asm for PIC 12F675   with MPLAB X / mpasm
;
;	LM35で計測した温度を、シリアル経由で送出するプログラム
;	（テスト用）
;	
;	GP0 ： 出力 （動作表示用LED）
;	GP1 ： 出力 （RS232C信号)
;	GP2 ： 入力 （RS232C信号）　未実装
;	GP4 ： 受信 （LM35からのアナログデータ)
;
;	データ形式　： "TEMP xx yyyy<LF>"	xx=カウンタ、yyyy=温度、<LF>=0x0a
;
;	バージョン V02
;	program by 井上 博計 / MAR.20.2005
;	動作保証無。転載、改変自由

	list			p=pic12f675	; プロセッサ名変更 PIC12F625 へ
	#include		<P12F675.INC>	; プロセッサに合わせて、INCファイル変更
	errorlevel	-302			; エラーレベル追加


	__CONFIG   _CP_OFF & _CPD_OFF & _BODEN_OFF & _MCLRE_OFF & _WDT_OFF & _PWRTE_ON & _INTRC_OSC_NOCLKOUT 



; 変数のメモリへのマッピング
	CBLOCK	0x20				; 12F675 の BANK0 メモリはアドレス 0x20〜0x5f が利用可能
	CNT1
	CNT2
	CNT3
	BUF
	_COUNTER				; テスト用カウンター
	_RS232_TMPN1
	NUM_CHAR10				; 数字を文字列に変換する時に利用する
	NUM_CHAR1
	ENDC

;***************************
; プログラム開始
;***************************
	ORG		0x000
	GOTO		PROGRAM_START
	
	ORG		0x004			; 不意の割り込みが発生した場合
	RETFIE

PROGRAM_START
	; GPIO の IN/OUT 設定 (GP1 を OUTPUT, GP4-AN3 を INPUT)
 	BSF		STATUS, RP0		; Select Bank 1
	MOVLW	B'00010000'		; bit=0:出力、bit=1:入力
	MOVWF	TRISIO

	; GPIO GP0 〜 GP2 のコンパレータ設定 (OFFとする)
	BCF		STATUS, RP0		; Select Bank 0
 	MOVLW	0x07				; コンパレータを使わない
 	MOVWF	CMCON
 	
 	BSF		STATUS, RP0		; Select Bank 1
 	MOVLW	B'00011000'		; A/D変換のクロックとポート
							; Fosc/8, IN=AN3
 	MOVWF	ANSEL
	BCF		STATUS, RP0		; Select Bank 0

	BCF		INTCON, PEIE		; 周辺割り込みすべてOFF

	MOVLW	B'00000010'
	MOVWF	GPIO				; 未送信時はつねに1を送りつづける

	;***************************
	; 電源投入後、3秒待つ
	;***************************
	CALL		DELAY_1SEC		; 1秒待ち
	CALL		DELAY_1SEC		; 1秒待ち
	CALL		DELAY_1SEC		; 1秒待ち

	CLRF		_COUNTER

	MOVLW	A'V'
	CALL		SEND232C_CHAR
	MOVLW	A'0'
	CALL		SEND232C_CHAR
	MOVLW	A'2'
	CALL		SEND232C_CHAR
	MOVLW	H'0a'
	CALL		SEND232C_CHAR

	
	;***************************
	; メイン・ループ
	;***************************

MAIN
	;***************************
	; 文字列　"TEMP " を RS232Cへ出力
	;***************************

	MOVLW	A'T'
	CALL		SEND232C_CHAR
	MOVLW	A'E'
	CALL		SEND232C_CHAR
	MOVLW	A'M'
	CALL		SEND232C_CHAR
	MOVLW	A'P'
	CALL		SEND232C_CHAR
	MOVLW	A' '
	CALL		SEND232C_CHAR

	;***************************
	; カウンター "00" をRS232Cへ出力 (２桁の１６進数文字列)
	;***************************
	MOVF		_COUNTER, W
	CALL		SEND232C_NUM
	MOVLW	A' '
	CALL		SEND232C_CHAR
	
	INCF		_COUNTER, F		; _COUNTER++

	;***************************
	; 温度データをA/D変換
	;***************************
	MOVLW	B'10001101'		; AD変換開始　（右桁そろえ、AN3)
	MOVWF	ADCON0
	CALL		DELAY_27USEC
	BSF		ADCON0, 1		; GOビット=1 で変換開始
AD_WAIT_LOOP
	BTFSC	ADCON0, 1		; GOビット=1はまだ変換中。=0は次の命令をスキップ
	GOTO AD_WAIT_LOOP
	
	;***************************
	; 温度データ"0000" をRS232Cへ出力 (４桁の１６進数文字列)
	;***************************
	MOVF		ADRESH, W
	CALL		SEND232C_NUM
 	BSF		STATUS, RP0		; Select Bank 1
	MOVF		ADRESL, W
	BCF		STATUS, RP0		; Select Bank 0
	CALL		SEND232C_NUM
	
	;***************************
	; 改行コードをRS232Cへ送出
	;***************************
	MOVLW	H'0a'
	CALL		SEND232C_CHAR

	;***************************
	; 動作確認用のLEDを点灯  (LED点灯・消灯で１秒かせぐ)
	;***************************
	MOVLW	B'00000011'				; LED 点灯
	MOVWF	GPIO
	CALL		DELAY_100MSEC		; 0.1秒待ち
	
							; LED 消灯
	MOVLW	B'00000010'
	MOVWF	GPIO

	CALL		DELAY_900MSEC		; 0.9秒待ち

	GOTO		MAIN

;***************************
; ここからサブルーチン部
;***************************

;***************************
; 1文字をRS232Cへ送出 （送出文字はWレジスタで受け取る）
;***************************
SEND232C_CHAR
	MOVWF	BUF				; 送出すべき文字（8bit）を BUFに入力
	
	MOVLW	B'00000000'		; スタートビット 0 を送出
	MOVWF	GPIO

	CALL		DELAY_417USEC		; 417マイクロ秒待つ (2400bps)
	
	MOVLW	D'8'				; 送出する文字データは 8bit
	MOVWF	CNT1

	

							; データを下位ビットから、１つずつ送る
SEND232C_CHAR_LOOP
	MOVLW	B'00000000'
	BTFSC	BUF, 0			; BUFのbit0が0なら、次の行をスキップ
	MOVLW	B'00000010'
	MOVWF	GPIO

	CALL		DELAY_417USEC		; 417マイクロ秒待つ (2400bps)

	RRF		BUF, F			; BUFを右へ１ビットずらし、結果をBUFに書き戻す
	DECFSZ	CNT1, F			; CNT1=CNT1-1、結果が0なら次の行はスキップ
	GOTO		SEND232C_CHAR_LOOP

	MOVLW	B'00000010'		; ストップビット 1 を送出
	MOVWF	GPIO
 
	CALL		DELAY_417USEC		; 417マイクロ秒待つ (2400bps)

	RETURN
 

;***************************
; 1バイトの数字を、２桁の１６進数文字列で送信(末尾改行なし)
; （送出する数字データはWレジスタで受け取る）
;***************************
SEND232C_NUM
	MOVWF	_RS232_TMPN1		; 送出すべき文字（8bit）を _RS232_TMPN1に入力
	CLRF		NUM_CHAR10		; 各桁を入れるバッファをクリアする
	CLRF		NUM_CHAR1

	;***************************
	; 10の位の文字化
	;***************************

	MOVF		_RS232_TMPN1, W	; W=_RS232_TMPN1
	MOVWF	CNT1				; CNT1=W
	
	SWAPF	CNT1, F			; CNT1の上4bit、下4bitを入れ替える
	MOVLW	B'00001111'		; W=b'00001111'
	ANDWF	CNT1, F			; CNT1=CNT1&&W  下4bitを取り出す
	
	MOVF		CNT1, W			; W=CNT1
	SUBLW	H'09'			; W=0x09-W  CNT1が0〜9のときCF=1
	BTFSS	STATUS, 0		; CF=1 の時、次行をスキップ
	GOTO		SEND232C_NUM_1
							; 0〜9の処理
	MOVLW	A'0'				; W=(char)'0'
	ADDWF	CNT1, F			; CNT1=CNT1+W
	GOTO		SEND232C_NUM_2
SEND232C_NUM_1
							; A〜Fの処理
	MOVLW	D'10'			; W=10
	SUBLW	A'A'				; W=(char)'A'-W
	ADDWF	CNT1, F			; CNT1=CNT1+W
SEND232C_NUM_2

	MOVF		CNT1, W			; W=CNT1
	MOVWF	NUM_CHAR10		; NUM_CHAR10=W  10の位確定

	CALL		SEND232C_CHAR		; 1文字送信


	;***************************
	; 1の位の文字化
	;***************************
	MOVF		_RS232_TMPN1, W	; W=_RS232_TMPN1
	MOVWF	CNT1				; CNT1=W
	
	MOVLW	B'00001111'		; W=b'00001111'
	ANDWF	CNT1, F			; CNT1=CNT1&&W  下4bitを取り出す
	
	MOVF		CNT1, W			; W=CNT1
	SUBLW	H'09'			; W=0x09-W  CNT1が0〜9のときCF=1
	BTFSS	STATUS, 0		; CF=1 の時、次行をスキップ
	GOTO		SEND232C_NUM_3
							; 0〜9の処理
	MOVLW	A'0'				; W=(char)'0'
	ADDWF	CNT1, F			; CNT1=CNT1+W
	GOTO		SEND232C_NUM_4
SEND232C_NUM_3
							; A〜Fの処理
	MOVLW	D'10'			; W=10
	SUBLW	A'A'				; W=(char)'A'-W
	ADDWF	CNT1, F			; CNT1=CNT1+W
SEND232C_NUM_4

	MOVF		CNT1, W			; W=CNT1
	MOVWF	NUM_CHAR1		; NUM_CHAR10=W  1の位確定

	CALL		SEND232C_CHAR		; 1文字送信
	

	RETURN

;***************************
; 特定秒数をかせぐ、ウェイトサブルーチン
;***************************

								; 1命令サイクル = 4クロック
								; 4MHzクロック時は、１命令は1MHz(=1usec)

;***************************
; 27マイクロ秒待つ (A/D変換)
DELAY_27USEC
	MOVLW	D'8' 
	MOVWF	CNT2
DELAY_27USEC_LOOP	
	DECFSZ	CNT2,F
	GOTO		DELAY_27USEC_LOOP
	RETURN

;***************************
; 417マイクロ秒待つ (2400bps)
DELAY_417USEC
	MOVLW	D'80' 
	MOVWF	CNT2
DELAY_417USEC_LOOP	
	GOTO		$+1
	DECFSZ	CNT2,F
	GOTO		DELAY_417USEC_LOOP
	RETURN

;***************************
; 50ミリ秒待つ
DELAY_50MSEC
	MOVLW	D'50'
	MOVWF	CNT1
DELAY_50MSEC_LOOP0				; ((4*250)+5)*50サイクル = 50250サイクル
	MOVLW	D'250'
	MOVWF	CNT2
DELAY_50MSEC_LOOP1				; 4*250サイクル
	NOP							; 1
	DECFSZ	CNT2, F				; 1(no skip), 2
	GOTO 	DELAY_50MSEC_LOOP1	; 2
	DECFSZ	CNT1, F				; 1(no skip), 2
	GOTO 	DELAY_50MSEC_LOOP0	; 2
	RETURN

;***************************
; 1秒待つ
DELAY_1SEC
	MOVLW	d'20'
	MOVWF	CNT3				; 50msec * 20
DELAY_1SEC_LOOP
	CALL	DELAY_50MSEC
	DECFSZ	CNT3, F
	GOTO	DELAY_1SEC_LOOP
	RETURN

;***************************
; 0.5秒待つ
DELAY_500MSEC
	MOVLW	d'10'
	MOVWF	CNT3				; 50msec * 10
DELAY_500MSEC_LOOP
	CALL	DELAY_50MSEC
	DECFSZ	CNT3, F
	GOTO	DELAY_500MSEC_LOOP
	RETURN

;***************************
; 0.9秒待つ
DELAY_900MSEC
	MOVLW	d'18'
	MOVWF	CNT3				; 50msec * 18
DELAY_900MSEC_LOOP
	CALL	DELAY_50MSEC
	DECFSZ	CNT3, F
	GOTO	DELAY_900MSEC_LOOP
	RETURN

;***************************
; 0.1秒待つ
DELAY_100MSEC
	MOVLW	d'2'
	MOVWF	CNT3				; 50msec * 2
DELAY_100MSEC_LOOP
	CALL	DELAY_50MSEC
	DECFSZ	CNT3, F
	GOTO	DELAY_100MSEC_LOOP
	RETURN


	END
