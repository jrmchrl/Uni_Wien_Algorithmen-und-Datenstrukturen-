#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <math.h>
#define nullptr 0

/*
	Notizen Seperate Chaining:
	- Statisches Hashverfahren, müssen aber beliebig viele Werte eingefügt werden können
	- Hashing dictionary = vector = hashtabelle and vice cersa
	- Randbedingung: n (mögliche Schlüsselwerte) >> m (Plätze in Tabelle --> Muss: Primzahl), Menge K von n Schlüsseln
	- Hashfunktion:Gegebener Wert n mod m = Rest der Teilung von n/m = Index
	- Kollision: stetiger Speicheradressenwechsel des Wertes welcher nach hinten verschoben wird durch einfügen neuen Wertes, speicherung der adresse des Wertes, welcher vorher an Stelle stand
*/

template <typename Key, size_t N = 1> //Anfangsgroeße muss gegeben werden = Separate Chaining ? Anfangsgroeße der Hashtabelle
class ADS_set
{
public:
	class Iterator;
	using value_type = Key;
	using key_type = Key;
	using reference = key_type &;
	using const_reference = const key_type &;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	using iterator = Iterator;
	using const_iterator = Iterator;
	// using key_compare = std::less<key_type>;   // B+-Tree
	using key_equal = std::equal_to<key_type>; // Hashing
	using hasher = std::hash<key_type>;		   // Hashing

private:
	// Node: der Key n und Verbindung enthält 
	struct node //einzufügendes Element mit Wert und Zeiger auf nächsten Wert
		{
			key_type wert; //Wert
			node *next;    //Zeiger auf nächsten Wert in Kette
						   //key_type wert = Item item; node *next stays the same; wert = allokierter Speicher für zu speichernden Wert
		node(key_type x, node *t)    //Initialisierung auf Wert x; Zeigerfeld ist zeiger auf nächstes element
			{
				wert =  x;
				next = t;
				};
		~node(){
			delete next; //std::cerr << "delete: node-destruktor node" << std::endl;
						 // Destructor enables to delete node, when called via "delete node"
			}
		}**tableofheads = nullptr; // legt node als head im tableofheads an - double pointer auf node-Objekt
	//typedef node *link;                   	// Variable vom Typ node wird deklariert, typedef  schlüsselwort, das zur Erstellung eines Alias für Datentyp verwendet wird.
	
	

	// Anzahl der elemente aller verketteten Listen 
    size_type totalsize = 0;

	// Anzahl der intern gespeicherten Listen/ Ketten 
    size_type tablesize = N;

	double max_lf = 0.7;


	void init()   
	// initialisieren - Hilfsfunktion für tableofheads - ++k so lange k != tablesize; Initialisierung des array mit nullptr
		{
			tableofheads = new node*[tablesize]; //TODO Anzahl der Heads abhängig von Tablesize?? 
			for(size_type k = 0;k != tablesize; ++k) 
			{
				tableofheads[k]=nullptr;
			}
		}


	size_type hash_function(const key_type &key) const
	// Hashfunktion gibt Index für Wert in tableofheads zurück
    {
        return hasher{}(key) % tablesize; // TODO warum steht Funktion außerhalb der geschweiften Klammern?
    }

	// returned true, falls n prim ist
    bool isPrime(size_type n)
    {
        for (size_type i = 2; i <= n / 2; i++)
        {
            if (n % i == 0)
            {
                return false;
            }
        }
        return true;
    }

	// vergroeßert n solang, bis n prim ist
    size_type nextPrime(size_type n)
    {
        //int debug_counter = 1;
        while (!isPrime(n))
        {
            n++;
        }
       
        return n;
    }


	ADS_set(node** table, size_type tablesize, size_type totalsize)
	{
		tableofheads = table;
		this->tablesize = tablesize;
		this->totalsize = totalsize;
	}

	// pruefen ob rehash durchgefuehrt werden muss
    void reserve(size_type n)
    {
        if (n > tablesize * max_lf)
        {
            //size_type new_tablesize = tablesize;
            // performance gewinn oder verlust?
            //new_tablesize = nextPrime(new_tablesize * 6 + 1);
            rehash(nextPrime(tablesize * 6 + 1));
        }
    }


    


public:

	void rehash(size_type n)
	{
		ADS_set old(tableofheads, tablesize, totalsize);
		// size_type tablesize_old = tablesize;
		tablesize = std::max(n, N);

		// tabellen setzen 
		//node** tableofheads_old = tableofheads;
		//ADS_set::iterator itbegin = begin(); //Iterator zeigt auf Anfang
		//ADS_set::iterator itend = end(); //Iterator zeigt auf Ende
		init();
		insert(old.begin(), old.end()); //wenn Funktion zuende ist wird Objekt gelöscht
    }

	ADS_set() 
	// Default-Konstruktor. Erzeugt einen leeren Container.�Komplexit�t: O(1)
		{
			init();
		}


	ADS_set(std::initializer_list<key_type> ilist) 
	// Initializer - List - Konstruktor.Erzeugt einen Container, der die Elemente aus�ilist�enth�lt.Die Elemente werden in der in�ilist�vorgegebenen Reihenfolge eingef�gt. 
	// Komplexit�t: B + -Baum : O(ilist_size * log ilist_size), Hashing : O(ilist_size)
	{
			init();
			insert(ilist);

	}

	template <typename InputIt>
	ADS_set(InputIt first, InputIt last)
	//Range-Konstruktor. Erzeugt einen Container, der die Elemente aus dem Bereich�[first, last[�enth�lt. 
	// Die Elemente werden in der vorgegebenen Reihenfolge (beginnend mit�first) eingef�gt.
	// �Komplexit�t: B + -Baum : O(range_size * log range_size), Hashing : O(range_size)
	{
		init();	
		insert(first, last);
	}

	ADS_set(const ADS_set& other)
	// Kopierkonstruktor. Erzeugt einen Container, der dieselben Elemente wie der Container�other�enth�lt. 
	// Komplexit�t: B + -Baum : O(other_size * log other_size)[2], Hashing : O(other_size) 
	// Werte werden wieder neu in größerer Hashtabelle eingefügt der Reihe nach, wenn alter zu klein ist
	{
		init();
		*this=other;
	}


	~ADS_set() // Die Destruktoren der gespeicherten Elemente werden aufgerufen und der ben�tzte Speicherplatz vollst�ndig freigegeben.�Komplexit�t: O(size)
	{
		loeschen();
	}

	

	// --------------------------------------------------------------------------------------------------- x -----------------------------------------------------------------------------
	
	void loeschen()

	{
		if (tableofheads != nullptr)
		{
			for(size_type k = 0;k != N; ++k)
			{
				delete tableofheads[k]; //löscht ganzen Table of Heads of list
			}
			delete[] tableofheads;
			tableofheads = nullptr;
		}
		totalsize = 0;
		tablesize = 0;
	}

	ADS_set& operator=(const ADS_set& other)
	// Kopierzuweisungsoperator. Der Inhalt des Containers wird durch den Inhalt von�other�ersetzt.
	//�R�ckgabewert: Referenz auf�* this�;Komplexit�t : B + -Baum O(size + other_size * log other_size)[2], Hashing : O(size + other_size)
	{
		loeschen();
		tablesize = other.tablesize;
		init();
		for (size_type k = 0; k != tablesize; ++k)
		{
			// Am Beginn jeder Liste verwalten wir einen Pseudoknoten, einen so genannten Kopfknoten (headnode). Im Kopfknoten einer Liste
			// ignorieren wir das Elementfeld, verwalten aber seine Referenz als zeiger
			// auf den Knoten, der das erste ELement in der Liste enthält
			node* head = nullptr;
			node* otherhead = other.tableofheads[k];
			while(otherhead!=nullptr)
			{
				head = new node(otherhead->wert,head);
				otherhead = otherhead->next;
			}
			tableofheads[k] = head;
		}
		return *this;

	}
	
	ADS_set& operator=(std::initializer_list<key_type> ilist)
	// Initializer-List-Zuweisungsoperator. Der Inhalt des Containers wird durch die Elemente aus�ilist�ersetzt. 
	//Die Elemente werden in der in�ilist�vorgegebenen Reihenfolge eingef�gt.�R�ckgabewert: Referenz auf�* this�Komplexit�t : B + -Baum O(size + ilist_size * log ilist_size), Hashing : O(size + ilist_size)
	{
		loeschen();
		insert(ilist);
		return *this;
	}
	
	
	size_type size() const // R�ckgabewert: Anzahl der im Container gespeicherten Elemente�Komplexit�t: O(1) // size_type = universeller Ausgabetyp für int 
	{ 
		return totalsize;

	}

	
	bool empty() const 
	// R�ckgabewert:�true, wenn�size()==0,�false�sonst.�Komplexit�t: O(1)
	{
		return (totalsize == 0);
	} 

	size_type count(const key_type &key) const
	// R�ckgabewert: Die Anzahl der Elemente im Container mit Schl�ssel�key�(0 oder 1).	Komplexit�t : B + -Baum O(log size)), Hashing O(1)
	{ 
		size_type n = 0;
		node* aktuell = tableofheads[hash_function(key)];   
		while (aktuell!=nullptr)   // while in dump, weil for in dump erst size aufrufen müsste
		{
			if (key_equal{}(aktuell->wert, key))
            {++n;} 							//n wird erhöht, wenn Key schon vorhandne
			aktuell = aktuell->next;	 //Ausgabe nächster wert
		}
		return n;

	}
	
	/*Suche: Suchen: solange eine Zieladresse durch ein Element belegt ist, 
	und der gesuchte Schlüsselwert noch nicht gefunden wurd,e muss über den Kollisionspfad
	weitergesucht werden - 	Separate Chaining: Verfolgen der linearen Liste*/
	iterator find(const key_type& key) const 
	//R�ckgabewert: ein Iterator auf das Element mit dem Schl�ssel�key�bzw. der End-Iterator (siehe�end()), falls kein solches Element vorhanden ist.
	//�Komplexit�t: B + -Baum O(log size), Hashing O(1)
	{
		size_type position = hash_function(key);
		node* aktuell = tableofheads[position];  
		while (aktuell!=nullptr)   // while in dump, weil for in dump erst size aufrufen müsste
		{
			if (key_equal{}(aktuell->wert, key))
            {
				return ADS_set::Iterator(tableofheads, aktuell, tablesize, position);
			} 							//n wird erhöht, wenn Key schon vorhandne
			aktuell = aktuell->next;	 //Ausgabe nächster wert
		}
		return const_iterator(nullptr);
	}
	
	void clear()
	// Entfernt alle Elemente aus dem Container. Komplexit�t: O(size)
	{
		loeschen();
	}


	void swap(ADS_set& other)
	// Vertauscht die Elemente des Containers mit den Elementen des Containers�other.
	// �Komplexit�t: O(1)
	{
		using std::swap;
		swap(tableofheads, other.tableofheads);  //mit Punkt wird auf instanziiertes Objekt einer Klasse zugegriffen - in diesem Fall Table
        swap(totalsize, other.totalsize);
        swap(tablesize, other.tablesize);
        //swap(max_lf, other.max_lf);

	}

	

/*Steps für Funktion Einfügen:
		1. Berechnung der Hashfunktion = Index von Vektor für key
		2. Speicher für Knoten zuordnen
		3. Knoten am Anfang der Liste einklinken
			Liste funktionieren alle als Stack, sodass wir leicht zuletzt eingefügten Elemente, 
			die sich am Anfang der Listen befinden, entfernen können
			*/
	void insert(std::initializer_list<key_type> ilist) 
	// F�gt die Elemente aus�ilist�ein. Die Elemente werden in der in�ilist�vorgegebenen Reihenfolge eingef�gt.
	// �Komplexit�t: B + -Baum : O(ilist_size * log(ilist_size + size)), Hashing : O(ilist_size)
	
	{
		for (key_type i : ilist)
		{
			
			if (count(i) == 0){
				++totalsize;
				node* oldHead = tableofheads[hash_function(i)];
				tableofheads[hash_function(i)] = new node(i, oldHead);
			}
		}

	}



	std::pair<iterator, bool> insert(const key_type& key) 
	//F�gt�key�ein.�R�ckgabewert: Ein�std::pair, bestehend aus einem�iterator�und einem�bool. iterator : zeigt auf das eingef�gte Element,
	//falls ein Element eingef�gt wurde, bzw.auf das bereits vorhandene Element mit demselben�key, falls kein Element eingef�gt wurde.
	// bool : true, falls eine Element eingef�gt wurde, false�sonst. Komplexit�t : B + -Baum : O(log size), Hashing O(1)
	{
		size_type index = hash_function(key);
		node* head = tableofheads[index];
		while(head!=nullptr)
		{
			if(key_equal{}(head->wert, key))
			{
				return std::pair<iterator, bool>(const_iterator (tableofheads, head, tablesize, index),false);
			}
		}
		tableofheads[index]= new node(key,tableofheads[index]);
		return std::pair<iterator, bool>(const_iterator (tableofheads, tableofheads[index], tablesize, index),true);
	}


	template <typename InputIt>
	void insert(InputIt first, InputIt last)// F�gt die Elemente aus dem Bereich�[first, last[�in der vorgegebenen Reihenfolge (beginnend mit�first) ein.� Komplexit�t: B + -Baum : O(range_size * log(range_size + size)), Hashing : O(range_size)

	{
		for(InputIt it = first;it != last; ++it)  // it = iterator(kümmert sich drum, dass immer so viele Bytes weitergezählt werden, wie das Objet zuvor gebraucht hat - ++it rutschts zum nächsten objekt weiter); it = Speicheradresse
		{
			if (count(*it) == 0){
				++totalsize;
				node* oldHead = tableofheads[hash_function(*it)];
				tableofheads[hash_function(*it)] = new node(*it, oldHead);    // it ist kein echter pointer - greift auf objekt zu, auf der der it hingesprungen ist
				} 
		}
	}



	size_type erase(const key_type& key) 
	// Entfernt das Element�key.�R�ckgabewert: Anzahl der gel�schten Elemente(0 oder 1). 
	//Komplexit�t : B + -Baum : O(log size), Hashing : O(1) Bei den dynamischen Hashverfahren ist es zur Vereinfachung zul�ssig, nur die jeweils betroffenen Werte aus den Buckets zu entfernen(kein Verschmelzen etc.n�tig).Die Datenstruktur muss nat�rlich funktionsf�hig bleiben.Bei den statischen Hashverfahren ist es nicht n�tig, eine vergr��erte Tabelle wieder zu verkleinern.

	{

		//Wäre eleganter, aber --it nicht möglich
		/*ADS_set::iterator it = find(key);
		if (it == end()){
			return 0;
		}else{
		//	de
		}
*/

		size_type found = 0;
		size_type index = hash_function(key);
		if(tableofheads[index] != nullptr)
		{
			node* head = tableofheads[index];
			node* prevhead = head;
			if(key_equal{}(head->wert , key))
			{
				tableofheads[index] = head->next;
				delete head;
				++found;
				--totalsize;
				return found;
			}
			while(head->next != nullptr)
			{
				prevhead = head;
				head = head->next;
				if(key_equal{}(head->wert, key))
				{
					prevhead->next = head->next;
					delete head;
					++found;
					--totalsize;
					return found;
				}
			}
		}
		return found;
	}


	iterator begin()  
	// Liefert einen Iterator auf das erste Element im Container.
	// �R�ckgabewert: Iterator auf des erste Element bzw.der End - Iterator, falls der Container leer ist. Wir legen hier Objekt der Iterator-Klasse weiter unten an
	{
		//dump();
		printf ("Begin non const %d %d", totalsize, tablesize);

		if (empty())
		{
			printf ("empty");
			return const_iterator (nullptr);
		}
		else
		{
			printf ("elsebegin");
			size_type index = 0;
			while (index<tablesize && tableofheads[index] == nullptr)
			{
				printf ("whilebegin");
				++index;
			}printf ("afterwhile");
			if(index == tablesize){
				printf ("Beginsecondif");
				return const_iterator (nullptr);
			}
			return ADS_set::Iterator(tableofheads, tableofheads[index], tablesize, index);
		}
		
	}

	const_iterator begin() const 
	// Liefert einen Iterator auf das erste Element im Container.
	// �R�ckgabewert: Iterator auf des erste Element bzw.der End - Iterator, falls der Container leer ist. Wir legen hier Objekt der Iterator-Klasse weiter unten an
	{
		//dump();
		printf ("Begin const %d %d", totalsize, tablesize);

		if (empty())
		{
			printf ("empty");
			return const_iterator (nullptr);
		}
		else
		{
			printf ("elsebegin");
			size_type index = 0;
			while (index<tablesize && tableofheads[index] == nullptr)
			{
				printf ("whilebegin");
				++index;
			}printf ("afterwhile");
			if(index == tablesize){
				printf ("Beginsecondif");
				return const_iterator (nullptr);
			}
			return ADS_set::Iterator(tableofheads, tableofheads[index], tablesize, index);
		}
		
	}

	const_iterator end() const 
	// Liefert einen Iterator auf das (virtuelle) Element nach dem letzten Element im Container ("End-Iterator").�R�ckgabewert: End - Iterator�
	{
		return const_iterator(nullptr);
	}

	void dump(std::ostream &o = std::cerr) const 
	// Ausgabe des Containerinhalts auf den Stream�o. Es gibt keine Vorgabe zur Funktionalit�t von�dump(), d.h. es ist nicht vorgeschrieben, was die Methode in den Stream schreibt. Es ist auch zul�ssig, dass die Methode nichts ausgibt. Es empfiehlt sich aber, zumindest alle enthaltenen Elemente auszugeben. Beim Testen kann es dar�ber hinaus hilfreich sein, wenn die Ausgabe in irgendeiner Form auch den Zustand der Datenstruktur darstellt. �Der Unit - Test gibt bei Fehlern den Inhalt des Containers mit Hilfe dieser Methode aus, um die Fehlersuche zu erleichtern.Wenn die Ausgabe allzu umfangreich ist, kann diese beim Unit - Test allerdings gek�rzt werden.Es darf davon ausgegangen werden, dass der Container beim Unit - Test nur mit Elementdatentypen(key_type) instanziert wird, die den Ausgabeoperator(<< ) unterst�tzen.
	{ 
		o << "Separate Chaining<" << typeid(Key).name() << ',' << N << " , tablesize = " << tablesize << "\n";

		for(size_type k = 0; k != tablesize; ++k)
		{
			node* aktuell = tableofheads[k];
			while (aktuell!=nullptr)  
			{
				o << aktuell ->wert <<" ";   //Ausgabe des aktuellen Wertes der Liste
				aktuell = aktuell->next;	 //Ausgabe nächster wert
			}
			o << "; "; //Wechsel zur nächsten Kette

		/*o << "\n";
        o << "current table size (num elements filed): " << aktuell << "\n";
        o << "current table size (max size): " << tablesize << "\n"; */
		}
		
		

	}
	friend bool operator==(const ADS_set& lhs, const ADS_set& rhs) // Annahme: vom gleichen Templateklassentyp
	{
		if (lhs.totalsize != rhs.totalsize)
		{
			return false;
		}else{
			for (ADS_set::iterator it = lhs.begin(); it != lhs.end(); ++it){


				//for (ADS_set::iterator it = begin(); it != end(); ++it)

				
				if(rhs.count(*it)==0){
					return false;
				}
			}
			return true;
		}
	}

	friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs) 
	{
		return !(lhs==rhs);
	}
};

// --------------------------------------------------------------------------------------------------- ITERATOR -----------------------------------------------------------------------------


template <typename Key, size_t N>
class ADS_set<Key, N>::Iterator {
public:
	using value_type = Key;
	using difference_type = std::ptrdiff_t;
	using reference = const value_type&;
	using pointer = const value_type*;
	using iterator_category = std::forward_iterator_tag;

	explicit Iterator(node** table = nullptr, node* head = nullptr, size_type tablesize = 0, size_type index = 0) : table{table}, head{head}, tablesize{tablesize}, index{index}
	//Konstruktor - muss Tabelle kennen; hat ganze Tabelle, ein Kettenglied, listengröße, index / Länge der neuen Tabelle
	{
		//this->table = table;
		//this->head = head;
		//this->tablesize = tablesize;
		//this->index = index;
	} 
	reference operator*() const
	//soll uns Wert von aktuellem Node geben
	{
		return head->wert;
	}
	pointer operator->() const
	//soll uns pointer aufn aktuellen Node geben
	{
		return &(head->wert);
	}
	Iterator& operator++()  //wird nicht aufgerufen nutzt begin(), aber nicht ++methode nicht und deswegen selbes wort pausenlos ausgegeben  -- hab ausgaben
	{
		printf("iterator if");
		if (head != nullptr && head->next != nullptr)
		{
			printf("iterator if");
			head = head->next;
		}else{
			//std::cout << "iterator else";
			head = nullptr; 
			bool found = false;
			
			while (head == nullptr && index+1 < tablesize)
			{
				//std::cout << "iterator while" << " " << index;
				++index; 
				head = table[index]; //neue Zeile, davon erstes ELement rausholen
				found = true;
			}
			if(found == false)
			{
				head = nullptr;
			}
		}
		return *this;
	}

	Iterator operator++(int n)
	{
		for (int j = 0; j != n; ++j)
		{
			this->operator++();
	
		}
		return *this; //*this = selbstreferenz des Objektes
	}

	friend bool operator==(const Iterator& lhs, const Iterator& rhs) // Wenn ich zwei Iteratoren habe, zeigen beide auf selbes Objekt?
	{
		return (lhs.head == rhs.head);
	}

	friend bool operator!=(const Iterator& lhs, const Iterator& rhs) //Sind sie verschieden?
	{
		return (lhs.head != rhs.head);
	}

private: 
	node** table;
	node* head;
	size_type tablesize;
	size_type index;
};

template <typename Key, size_t N> void swap(ADS_set<Key, N>& lhs, ADS_set<Key, N>& rhs) { lhs.swap(rhs); }


#endif // ADS_SET_H#pragma once
