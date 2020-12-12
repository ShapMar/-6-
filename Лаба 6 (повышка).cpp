#include <windows.h>
#include <conio.h>
#include <time.h>
#include  <iostream>

int quan_reader = 0;
CRITICAL_SECTION cs;
HANDLE s_qr; //семафор со счётчиком читателей
HANDLE s_db;  //семафор с базой данных
int data_base = 0;

void reader()
{
	while (TRUE) {
		WaitForSingleObject(s_qr, INFINITE); //ждём доступа к счётчику
		quan_reader = quan_reader + 1;
		if (quan_reader == 1) //если данный поток пришёл первым, то получаем доступ к базе данных
			WaitForSingleObject(s_db, INFINITE);
		ReleaseSemaphore(s_qr, 1, NULL); //освобождаем счётчик		
		WaitForSingleObject(s_qr, INFINITE); //ждём доступа к счётчику
		quan_reader = quan_reader - 1;
		if (quan_reader == 0) //если данный поток вышел последним, то освобождаем семафор с базой данных
			ReleaseSemaphore(s_db, 1, NULL);
		ReleaseSemaphore(s_qr, 1, NULL); //освобождаем счётчик											
		EnterCriticalSection(&cs);
		std::cout << "Ч: " << data_base << std::endl; //читаем		
		LeaveCriticalSection(&cs);
		Sleep(50);
	}
}

void writer()
{
	while (TRUE) {
		WaitForSingleObject(s_db, INFINITE); //ждём, когда база данных будет свободна
		data_base = rand() % 10; //пишем новые данные
		ReleaseSemaphore(s_db, 1, NULL); //покидаем семафор		
		EnterCriticalSection(&cs);
		std::cout << "П: " << data_base << std::endl;
		LeaveCriticalSection(&cs);
		Sleep(100);
	}
}

int main()
{
	srand(time(0));
	setlocale(LC_ALL, "russian");
	InitializeCriticalSection(&cs);
	s_qr = CreateSemaphore(NULL, 1, 1, NULL);
	s_db = CreateSemaphore(NULL, 1, 1, NULL);
	// DWORD 32-битное беззнаковое целое
	// HANDLE - дескриптор
	HANDLE reader1;
	DWORD reader1ID;
	HANDLE reader2;
	DWORD reader2ID;
	HANDLE reader3;
	DWORD reader3ID;
	HANDLE writer0;
	DWORD writerID;
	reader1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)reader, NULL, 0, &reader1ID);
	reader2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)reader, NULL, 0, &reader2ID);
	reader3 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)reader, NULL, 0, &reader3ID);
	writer0 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)writer, NULL, 0, &writerID);
	if ((!reader1) || (!reader2) || (!reader3) || (!writer0))
	{
		std::cout << "Ошибка создания потока";
			return -1;
	}
	int ms = 300;//сколько будет работать программа в мс (ну, из-за функций sleep в reader и writer не совсем в мс)	
	WaitForSingleObject(reader1, ms);
	WaitForSingleObject(reader2, ms);
	WaitForSingleObject(reader3, ms);
	WaitForSingleObject(writer0, ms);
	DeleteCriticalSection(&cs);
	return 0;
}
