/*

* SOHET Maxime & SOMERS Harold
* Computer Networks
* Projet : Protocoles de réseaux
* tests.c
* 26/10/2015
*/

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//#include "receiver.h"
//#include "sender.h"
//#include "main.h"
#include "packet_interface.h"

/*
*	Vérifie que après appelle à la fonction pkt_new(), le packet est alloué
*/
void TestPktNew()
{
	  pkt_t *test = pkt_new();
	  CU_ASSERT_PTR_NOT_NULL(test);
	  pkt_del(test);
}

/*
*	Vérifie que après appelle à la fonction pkt_del(), le packet est free
*/
void TestPktDel()
{
	pkt_t *test = pkt_new();	
	pkt_del(test);
	CU_ASSERT_PTR_NULL(test);
}

/*
* Test de la fonction pkt_set_type
*/
void TestPktSetType()
{
	pkt_t *test_pkt = pkt_new();
	
	pkt_status_code test1 = pkt_set_type(test_pkt,(ptypes_t) 3);
	CU_ASSERT_EQUAL(test1,E_TYPE);
	pkt_status_code test2 = pkt_set_type(test_pkt,(ptypes_t) 4);
	CU_ASSERT_EQUAL(test2,PKT_OK);
	pkt_status_code test3 = pkt_set_type(test_pkt,(ptypes_t) 1);
	CU_ASSERT_EQUAL(test3,PKT_OK);
	pkt_status_code test4 = pkt_set_type(test_pkt,(ptypes_t) 2);
	CU_ASSERT_EQUAL(test4,PKT_OK);
}

/*
* Test de la fonction pkt_set_window
*/
void TestPktSetWindow()
{
	pkt_t *test_pkt = pkt_new();
	
	pkt_status_code test1 = pkt_set_window(test_pkt,(uint8_t) 30);
	CU_ASSERT_EQUAL(test1,PKT_OK);
	pkt_status_code test2 = pkt_set_window(test_pkt,(uint8_t) 32);
	CU_ASSERT_EQUAL(test2,E_WINDOW);
}

/*
* Test de la fonction pkt_set_length
*/
void TestPktSetLength()
{
	pkt_t *test_pkt = pkt_new();
	
	pkt_status_code test1 = pkt_set_length(test_pkt,(uint16_t) 511);
	CU_ASSERT_EQUAL(test1,PKT_OK);
	pkt_status_code test2 = pkt_set_length(test_pkt,(uint16_t) 513);
	CU_ASSERT_EQUAL(test2,E_LENGTH);
}

/*
* Test de la fonction pkt_set_payload
*/
void TestPktSetPayload()
{
	pkt_t *test_pkt = pkt_new();
	pkt_status_code test1 = pkt_set_payload(test_pkt,"bkj",(uint16_t) 3);
	CU_ASSERT_EQUAL(test1,PKT_OK);
}

/*
* Tests de la fonction decode avec des valeurs arbitraires
*/
void TestPktDecode()
{
	pkt_status_code test = pkt_decode(NULL,2,NULL);
	
	pkt_t *testbis = pkt_new();
	char *data = malloc(sizeof(char)*5);
	data = "Hellos";
	uint16_t length= 6;
	pkt_set_payload(testbis, data, length);
	pkt_set_window(testbis,(uint8_t) 3);
	pkt_set_type(testbis,(ptypes_t) 2);
	pkt_set_seqnum(testbis,(uint8_t) 70);
	char *buf = (char*) malloc(sizeof(char*)*2000);
	size_t len;
	len =(size_t)2000;
	pkt_encode(testbis,buf,&len);
	pkt_t *deco = pkt_new();
	size_t leng;
	leng =(size_t)16;
	if(pkt_decode(buf,leng,deco)!=PKT_OK)
	{
		printf("erreur decode");
	}
	CU_ASSERT_EQUAL(test,E_NOHEADER);
  CU_ASSERT_EQUAL(pkt_get_type(deco),pkt_get_type(testbis));
  CU_ASSERT_EQUAL(pkt_get_seqnum(deco),pkt_get_seqnum(testbis));
  CU_ASSERT_EQUAL(pkt_get_window(deco),pkt_get_window(testbis));
  CU_ASSERT_STRING_EQUAL(pkt_get_payload(deco),pkt_get_payload(testbis));	
}

/*
* Tests fonction decode : verifie si il y un probleme de type, les autres valeurs du header doivent être correctes
*/
void TestPktDecodeSiProblemeType()
{
	pkt_t *testbis = pkt_new();
	char *data = malloc(sizeof(char)*5);
	data = "Hellos";
	uint16_t length= 6;
	pkt_set_payload(testbis, data, length);
	pkt_set_window(testbis,(uint8_t) 3);
	pkt_set_type(testbis,(ptypes_t) 2);
	pkt_set_seqnum(testbis,(uint8_t) 70);
	char *buf = (char*) malloc(sizeof(char*)*2000);
	size_t len;
	len =(size_t)2000;
	pkt_encode(testbis,buf,&len);
	*buf = (char) 3; // modification du type (premier byte)
	pkt_t *deco = pkt_new();
	size_t leng;
	leng =(size_t)16; 
	pkt_status_code test = pkt_decode(buf,leng,deco);	
 	CU_ASSERT_NOT_EQUAL(pkt_get_type(deco),pkt_get_type(testbis)); // uniquement le type est modifié
	CU_ASSERT_EQUAL(pkt_get_seqnum(deco),pkt_get_seqnum(testbis));
  CU_ASSERT_EQUAL(pkt_get_window(deco),pkt_get_window(testbis));
  CU_ASSERT_STRING_EQUAL(pkt_get_payload(deco),pkt_get_payload(testbis));	
}

/*
* Tests fonction decode : verifie si il y un probleme de window, les autres valeurs du header doivent être correctes
*/
void TestPktDecodeSiProblemeWindow()
{
	pkt_t *testbis = pkt_new();
	char *data = malloc(sizeof(char)*5);
	data = "Hellos";
	uint16_t length= 6;
	pkt_set_payload(testbis, data, length);
	pkt_set_window(testbis,(uint8_t) 3);
	pkt_set_type(testbis,(ptypes_t) 1);
	pkt_set_seqnum(testbis,(uint8_t) 70);
	char *buf = (char*) malloc(sizeof(char*)*2000);
	size_t len;
	len =(size_t)2000;
	pkt_encode(testbis,buf,&len);
	*buf = (char) 32; // modification de la window (premier byte)
	pkt_t *deco = pkt_new();
	size_t leng;
	leng =(size_t)16; 
	pkt_status_code test = pkt_decode(buf,leng,deco);	
 	CU_ASSERT_EQUAL(pkt_get_type(deco),pkt_get_type(testbis));
	CU_ASSERT_EQUAL(pkt_get_seqnum(deco),pkt_get_seqnum(testbis));
  CU_ASSERT_NOT_EQUAL(pkt_get_window(deco),pkt_get_window(testbis)); // uniquement la window est modifiée
  CU_ASSERT_STRING_EQUAL(pkt_get_payload(deco),pkt_get_payload(testbis));	
}

/*
* Tests fonction decode : verifie si la seqnum est modifiée, les autres valeurs du header sont correctes
*/
void TestPktDecodeSiProblemeSeqnum()
{
	pkt_t *testbis = pkt_new();
	char *data = malloc(sizeof(char)*5);
	data = "Hellos";
	uint16_t length= 6;
	pkt_set_payload(testbis, data, length);
	pkt_set_window(testbis,(uint8_t) 3);
	pkt_set_type(testbis,(ptypes_t) 1);
	pkt_set_seqnum(testbis,(uint8_t) 3);
	char *buf = (char*) malloc(sizeof(char*)*2000);
	size_t len;
	len =(size_t)2000;
	pkt_encode(testbis,buf,&len);
	*(buf+1) = (char) 1; // modification de la seqnum (second byte)
	pkt_t *deco = pkt_new();
	size_t leng;
	leng =(size_t)16; 
	pkt_status_code test = pkt_decode(buf,leng,deco);	
 	CU_ASSERT_EQUAL(pkt_get_type(deco),pkt_get_type(testbis));
	CU_ASSERT_NOT_EQUAL(pkt_get_seqnum(deco),pkt_get_seqnum(testbis)); // uniquement seqnum est modifié
  CU_ASSERT_EQUAL(pkt_get_window(deco),pkt_get_window(testbis));
  CU_ASSERT_STRING_EQUAL(pkt_get_payload(deco),pkt_get_payload(testbis));	
}


/*
* Tests avec des valeurs arbitraires de la fonction encode
*/
void TestPktEncode()
{		
	pkt_t *test = pkt_new();
	char *data = malloc(sizeof(char)*5);
	data = "talle";
	uint16_t length= (6);
	pkt_set_payload(test, data, length);
	pkt_set_window(test,(uint8_t) 6);
	pkt_set_type(test,(ptypes_t) 2);
	pkt_set_seqnum(test,(uint8_t) 9);
	char *buf = (char*) malloc(sizeof(char*)*20000000);
	size_t len;
	len =(size_t)20000000;
	pkt_encode(test,buf,&len);
	CU_ASSERT_EQUAL((int) *(buf),70);
	CU_ASSERT_EQUAL((int) *(buf+1),9);
	CU_ASSERT_EQUAL((int) *(buf+2),8);
	CU_ASSERT_EQUAL((int) *(buf+3),0);
	pkt_del(test);
	free(buf);
}

/*
* Tests encode() avec de mauvaises valeures pour le type
*/
void TestPktEncodeSiProblemeType()
{		
	pkt_t *test = pkt_new();
	char *data = malloc(sizeof(char)*5);
	data = "talle";
	uint16_t length= (6);
	pkt_set_payload(test, data, length);
	pkt_set_window(test,(uint8_t) 6);
	pkt_set_type(test,(ptypes_t) 2);
	pkt_set_seqnum(test,(uint8_t) 9);
	char *buf = (char*) malloc(sizeof(char*)*20000000);
	size_t len;
	len =(size_t)20000000;
	pkt_encode(test,buf,&len);
	*buf = (char) 3; // modification du type
	CU_ASSERT_NOT_EQUAL((int) *(buf),70);
	CU_ASSERT_EQUAL((int) *(buf+1),9);
	CU_ASSERT_EQUAL((int) *(buf+2),8);
	CU_ASSERT_EQUAL((int) *(buf+3),0);
	pkt_del(test);
	free(buf);
}

/*
* Tests encode() en modifiant le seqnum, les autres valeurs ne doivent pas être modifiée
*/
void TestPktEncodeSiProblemeSeqnum()
{		
	pkt_t *test = pkt_new();
	char *data = malloc(sizeof(char)*5);
	data = "talle";
	uint16_t length= (6);
	pkt_set_payload(test, data, length);
	pkt_set_window(test,(uint8_t) 6);
	pkt_set_type(test,(ptypes_t) 2);
	pkt_set_seqnum(test,(uint8_t) 9);
	char *buf = (char*) malloc(sizeof(char*)*20000000);
	size_t len;
	len =(size_t)20000000;
	pkt_encode(test,buf,&len);
	*(buf+1) = (char) 55; // modification de seqnum
	CU_ASSERT_EQUAL((int) *(buf),70);
	CU_ASSERT_NOT_EQUAL((int) *(buf+1),9);
	CU_ASSERT_EQUAL((int) *(buf+2),8);
	CU_ASSERT_EQUAL((int) *(buf+3),0);
	pkt_del(test);
	free(buf);
}

/*
* Tests pour la fonction real_address
*/
void TestRealAddress()
{
	struct sockaddr_in6 *retour = malloc(sizeof(struct sockaddr_in6*));
	const char address[] = "::1";
	const char *rval = real_address(address,retour);
	CU_ASSERT_PTR_NULL(rval);
	const char address2[] = "dg-8";
	const char *rval2 = real_address(address2,retour);
	CU_ASSERT_PTR_NOT_NULL(rval2);
}

/*
* Tests pour la fonction create_socket()
*/
void TestCreateSocket()
{
	struct sockaddr_in6 *retour = malloc(sizeof(struct sockaddr_in6*));
	int src_port = 61000;
	int dst_port = 62000;
	int err_port = -100; 
	const char local_address[] = "::1";
	real_address(local_address,retour);
	
	int socket1 = create_socket((struct sockaddr_in6 *) retour, err_port, NULL , 0);
	CU_ASSERT_EQUAL(socket1,-1);
	close(socket1);
	
	int socket2 = create_socket((struct sockaddr_in6 *) retour, src_port, NULL,0);
	CU_ASSERT(socket2>-1);
	close(socket2);
	
	int socket3 = create_socket((struct sockaddr_in6 *) retour, src_port, (struct sockaddr_in6 *)retour,dst_port);
	CU_ASSERT(socket3>-1);
	close(socket3);
}

/*
* La fonction main permet de déterminer les tests que nous décidons de vérifier
*/
 void main(int argc, const char *argv[])
 {
 	//Initialisation du registre
		if (CUE_SUCCESS != CU_initialize_registry()) 
			return CU_get_error(); 
	 
	 //Création d'une suite de test
		CU_pSuite s_pkt_new = CU_add_suite("suite_pkt_new",NULL,NULL),
							s_pkt_del = CU_add_suite("suite_pkt_del",NULL, NULL),
							s_pkt_set_type = CU_add_suite("suite_pkt_set_type",NULL, NULL),
							s_pkt_set_window = CU_add_suite("suite_pkt_set_window",NULL, NULL),
							s_pkt_set_length = CU_add_suite("suite_pkt_set_length",NULL, NULL),
							s_pkt_set_payload = CU_add_suite("suite_pkt_set_payload",NULL, NULL),
							s_pkt_encode = CU_add_suite("suite_pkt_encode",NULL, NULL),
							s_pkt_encode_si_probleme_type = CU_add_suite("suite_pkt_encode_si_probleme_type",NULL, NULL),
							s_pkt_encode_si_probleme_seqnum = CU_add_suite("suite_pkt_encode_si_probleme_seqnum",NULL, NULL),
							s_pkt_decode = CU_add_suite("suite_pkt_decode",NULL,NULL),
							s_pkt_decode_si_probleme_type = CU_add_suite("suite_pkt_decode_si_probleme_type",NULL, NULL),
							s_pkt_decode_si_probleme_window = CU_add_suite("suite_pkt_decode_si_probleme_window",NULL, NULL),
							s_pkt_decode_si_probleme_seqnum = CU_add_suite("suite_pkt_decode_si_probleme_seqnum",NULL, NULL),
							s_real_address = CU_add_suite("suite_real_address",NULL, NULL),
							s_create_socket = CU_add_suite("suite_create_socket",NULL, NULL);

		if (NULL == s_pkt_new 
				|| NULL == s_pkt_del
				|| NULL == s_pkt_set_type
				|| NULL == s_pkt_set_window
				|| NULL == s_pkt_set_length
				|| NULL == s_pkt_set_payload
				|| NULL == s_pkt_encode
				|| NULL == s_pkt_encode_si_probleme_type
				|| NULL == s_pkt_encode_si_probleme_seqnum
				|| NULL == s_pkt_decode
				|| NULL == s_pkt_decode_si_probleme_type
				|| NULL == s_pkt_decode_si_probleme_window
				|| NULL == s_pkt_decode_si_probleme_seqnum
				|| NULL == s_real_address
				|| NULL == s_create_socket)
		{
			CU_cleanup_registry();
			return CU_get_error();
		}
	
		if (NULL == CU_add_test(s_pkt_new,"TEST",TestPktNew)
				|| NULL == CU_add_test(s_pkt_del,"TEST",TestPktDel)
				|| NULL == CU_add_test(s_pkt_set_type,"TEST",TestPktSetType)
				|| NULL == CU_add_test(s_pkt_set_window,"TEST",TestPktSetWindow)
				|| NULL == CU_add_test(s_pkt_set_length,"TEST",TestPktSetLength)
				|| NULL == CU_add_test(s_pkt_set_payload,"TEST",TestPktSetPayload)
				|| NULL == CU_add_test(s_pkt_encode,"TEST",TestPktEncode)
				|| NULL == CU_add_test(s_pkt_encode_si_probleme_type,"TEST", TestPktEncodeSiProblemeType)
				|| NULL == CU_add_test(s_pkt_encode_si_probleme_seqnum,"TEST", TestPktEncodeSiProblemeSeqnum)
				|| NULL == CU_add_test(s_pkt_decode,"TEST",TestPktDecode)
				|| NULL == CU_add_test(s_pkt_decode_si_probleme_type,"TEST", TestPktDecodeSiProblemeType)
				|| NULL == CU_add_test(s_pkt_decode_si_probleme_window,"TEST", TestPktDecodeSiProblemeWindow)
				|| NULL == CU_add_test(s_pkt_decode_si_probleme_seqnum,"TEST", TestPktDecodeSiProblemeSeqnum)
				|| NULL == CU_add_test(s_real_address,"TEST", TestRealAddress)
				|| NULL == CU_add_test(s_create_socket,"TEST", TestCreateSocket))
		{
			CU_cleanup_registry();
			return CU_get_error();
		}
	
		CU_basic_run_tests();
		CU_cleanup_registry();
		
		return 0;
 }
