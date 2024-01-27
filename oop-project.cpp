#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <system_error>
#include <set>
#include <functional>
#include <chrono>
#include <list>
using namespace std;

enum SerializableType {
	RetetaType,
	PreparatType,
	ComandaType
};

class Utils {
public:
	static string trim(const string& source) {
		string s(source);
		s.erase(0, s.find_first_not_of(" \n\r\t"));
		s.erase(s.find_last_not_of(" \n\r\t") + 1);
		return s;
	}

	static bool startsWith(const string& str, const string& start) {
		return str.rfind(start, 0) == 0;
	}
};

class Serializable {
public:
	virtual ~Serializable() {}
	virtual void serialize(ostream& out, bool isTypeRequired) const = 0;
	virtual void deserialize(istream& in) = 0;
};

class ExceptionInput : public exception {
	string mesaj;

public:
	ExceptionInput(const string& mesaj) : mesaj(mesaj) {}

	const char* what() const noexcept override {
		return mesaj.c_str();
	}
};

class Restaurant;

class Reteta : public Serializable {
	string nume = "Reteta";
	map<string, int> ingredienteSiCantitate;

public:
	Reteta() {}

	Reteta(const string& nume, map<string, int> ingredienteSiCantitate) {
		if (nume.length() > 2) this->nume = nume;
		this->ingredienteSiCantitate = ingredienteSiCantitate;
	}

	Reteta(const string& nume) {
		if (nume.length() > 2) this->nume = nume;
	}

	Reteta(const Reteta& reteta) {
		this->nume = reteta.nume;
		this->ingredienteSiCantitate = reteta.ingredienteSiCantitate;
	}

	Reteta& operator=(const Reteta& reteta) {
		if (this != &reteta) {
			this->nume = reteta.nume;
			this->ingredienteSiCantitate = reteta.ingredienteSiCantitate;
		}

		return *this;
	}

	void setNume(const string& nume) {
		if (nume.length() > 2) this->nume = nume;
	}

	string getNume() const {
		return this->nume;
	}

	void modificaReteta(const string& ingredient, int cantitate) {
		if (ingredienteSiCantitate.find(ingredient) != ingredienteSiCantitate.end() && cantitate >= 0) {
			ingredienteSiCantitate[ingredient] = cantitate;
			return;
		}
		else if (ingredienteSiCantitate.find(ingredient) == ingredienteSiCantitate.end()) {
			ingredienteSiCantitate[ingredient] = cantitate;
			return;
		}

		throw new ExceptionInput("Cantitatea trebuie sa fie mai mare de 0");
	}

	void stergeIngredient(const string& ingredient) {
		auto it = ingredienteSiCantitate.find(ingredient);
		if (it != ingredienteSiCantitate.end()) {
			ingredienteSiCantitate.erase(it);
			return;
		}

		throw ExceptionInput("Ingredientul nu a fost gasit");
	}

	const map<string, int>& getIngredienteSiCantitate() const {
		return ingredienteSiCantitate;
	}

	friend ostream& operator<<(ostream& out, Reteta& reteta) {
		out << "Nume reteta: " << reteta.nume << endl;
		out << "   Ingrediente:";
		map<string, int>::iterator iterator = reteta.ingredienteSiCantitate.begin();
		while (iterator != reteta.ingredienteSiCantitate.end())
		{
			out << " | " << iterator->first << ", cantitate: " << iterator->second;
			iterator++;
		}
		cout << endl;
		cout << endl;
		return out;
	}

	friend ifstream& operator>>(ifstream& in, Reteta& reteta) {
		string line;
		getline(in, line);
		stringstream ss(line);

		getline(ss, reteta.nume, ',');
		reteta.nume = Utils::trim(reteta.nume);
		reteta.ingredienteSiCantitate.clear();

		string part;
		while (getline(ss, part, ',')) {
			stringstream parts_stream(part);
			string ingredient;
			int cantitate;

			parts_stream >> ingredient;
			parts_stream >> cantitate;

			ingredient = Utils::trim(ingredient);
			reteta.ingredienteSiCantitate[ingredient] = cantitate;
		}

		return in;
	}

	friend istream& operator>>(istream& in, Reteta& reteta) {
		while (true) {
			cout << "Introduceti numele retetei: ";
			string nume;
			getline(in, nume);
			string numeTrim = Utils::trim(nume);
			if (numeTrim.length() > 2) {
				reteta.nume = numeTrim;
				break;
			}
			else cout << "Numele retetei trebuie sa fie mai lung de 4." << endl;
		}

		int numarIngrediente;
		while (true) {
			cout << "Introduceti numarul de ingrediente (maxim 20): ";
			in >> numarIngrediente;

			if (in.fail()) {
				in.clear();
				cout << "Valoare invalida. Va rugam introduceti un numar." << endl;
				in.ignore(numeric_limits<streamsize>::max(), '\n');
				continue;
			}

			in.ignore(numeric_limits<streamsize>::max(), '\n');
			if (numarIngrediente <= 0 || numarIngrediente > 20) {
				cout << "Numarul de ingrediente trebuie sa fie intre 1 si 20." << endl;
			}
			else {
				break;
			}
		}

		reteta.ingredienteSiCantitate.clear();

		for (int i = 0; i < numarIngrediente; i++) {
			string ingredient;
			int cantitate;

			cout << "Introduceti ingredientul: ";
			getline(in, ingredient);
			string ingredientTrim = Utils::trim(ingredient);
			cout << "Introduceti cantitatea: ";
			in >> cantitate;

			if (in.fail()) {
				in.clear();
				--i;
				cout << "Valoare invalida." << endl;
			}
			else if (cantitate < 0) {
				--i;
				cout << "Cantitate trebuie sa fie mai mare de 0" << endl;
			}
			else if (ingredientTrim.length() < 3) {
				--i;
				cout << "Numele ingredientului trebuie sa fie mai lung de 3" << endl;
			}
			else if (reteta.ingredienteSiCantitate.find(ingredientTrim) != reteta.ingredienteSiCantitate.end()) {
				--i;
				cout << "Ingredient deja adaugat" << endl;
			}
			else {
				reteta.ingredienteSiCantitate[ingredientTrim] = cantitate;
			}
			in.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		return in;
	}

	friend bool operator==(Reteta& lreteta, Reteta& rreteta) {
		const auto& ingredienteL = lreteta.ingredienteSiCantitate;
		const auto& ingredienteR = rreteta.ingredienteSiCantitate;

		if (ingredienteL.size() != ingredienteR.size()) return false;

		for (const auto& elementL : ingredienteL) {
			auto elementR = ingredienteR.find(elementL.first);
			if (elementR == ingredienteR.end() || elementR->second != elementL.second) {
				return false;
			}
		}

		return true;
	}

	void serialize(ostream& out, bool isTypeRequired) const {
		if (isTypeRequired) {
			SerializableType type = RetetaType;
			out.write(reinterpret_cast<const char*>(&type), sizeof(type));
		}
		size_t length = nume.size();
		out.write(reinterpret_cast<const char*>(&length), sizeof(length));
		out.write(nume.c_str(), length);

		size_t mapSize = ingredienteSiCantitate.size();
		out.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
		for (const auto& pair : ingredienteSiCantitate) {
			length = pair.first.size();
			out.write(reinterpret_cast<const char*>(&length), sizeof(length));
			out.write(pair.first.c_str(), length);
			out.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
		}
	}

	void deserialize(istream& in) {
		size_t length;
		in.read(reinterpret_cast<char*>(&length), sizeof(length));
		nume.resize(length);
		in.read(&nume[0], length);

		size_t mapSize;
		in.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
		ingredienteSiCantitate.clear();
		for (size_t i = 0; i < mapSize; ++i) {
			in.read(reinterpret_cast<char*>(&length), sizeof(length));
			string ingredient;
			ingredient.resize(length);
			in.read(&ingredient[0], length);

			int cantitate;
			in.read(reinterpret_cast<char*>(&cantitate), sizeof(cantitate));
			ingredienteSiCantitate[ingredient] = cantitate;
		}
	}
};

class Preparat : public Serializable {
	static const int pretMaxim = 1000;
	string nume = "Preparat";
	float pret = 0;
	Reteta* reteta = nullptr;
	Restaurant* restaurant = nullptr;

public:
	Preparat() {}

	Preparat(Restaurant* restaurant);

	Preparat(const string& nume, float pret, Restaurant* restaurant);

	Preparat(const Preparat& preparat) {
		this->nume = preparat.nume;
		this->pret = preparat.pret;
		this->reteta = new Reteta(*preparat.reteta);
		this->restaurant = preparat.restaurant;
	}

	Preparat& operator=(const Preparat& preparat) {
		if (this != &preparat) {
			this->nume = preparat.nume;
			this->pret = preparat.pret;
			this->reteta = new Reteta(*preparat.reteta);
			this->restaurant = preparat.restaurant;
		}

		return *this;
	}

	~Preparat() {
		delete this->reteta;
		this->reteta = nullptr;
	}

	string getNume() const {
		return this->nume;
	}

	int getPret() const {
		return this->pret;
	}

	void setPret(float pret) {
		if (pret > 0 && pret < pretMaxim)
			this->pret = pret;
	}

	void setNume(const string& nume) {
		if (nume.length() > 2)
			this->nume = nume;
	}

	const Reteta* getReteta() const {
		return reteta;
	}

	Preparat operator+(int suma) {
		if (suma > 0) {
			Preparat preparat = *this;
			preparat.pret += suma;
			return preparat;
		}
	}

	friend ostream& operator<<(ostream& out, Preparat& p) {
		out << "-------------------------------" << endl;
		out << "Nume preparat: " << p.nume << ", pret: " << p.pret << endl;
		out << "  " << *p.reteta;
		out << "-------------------------------" << endl;

		return out;
	}

	friend istream& operator>>(istream& in, Preparat& p);

	friend ifstream& operator>>(ifstream& in, Preparat& p);

	void afiseazaPreparat(bool cuReteta) {
		cout << "Nume preparat: " << this->nume << ", pret: " << this->pret << endl;
		if (cuReteta)
			cout << "   " << *reteta;
	}

	static void proceseazaPreparat(const string& comanda, string& nume, float& pret) {
		istringstream iss(comanda);
		string firstToken, secondToken, thirdToken;

		if (!getline(iss, firstToken, ' ')) {
			throw ExceptionInput("Comanda invalida");
		}

		getline(iss, secondToken, ' ');

		if ((firstToken == "ADAUGA" && secondToken == "PREPARAT") || (firstToken == "MODIFICA" && secondToken == "PRET")) {
			if (firstToken == "MODIFICA" && secondToken == "PRET")
				getline(iss, thirdToken, ' ');

			if (!getline(iss, nume, ',')) {
				throw ExceptionInput("Nume invalid");
			}
			if (nume.length() < 6)
				throw ExceptionInput("Numele este prea scurt, minim 6 caractere");

			string pretStr;
			if (!getline(iss, pretStr)) {
				throw ExceptionInput("Pret invalid");
			}

			try {
				pret = stof(pretStr);
				return;
			}
			catch (const invalid_argument&) {
				throw ExceptionInput("Pret invalid");
			}
		}

		throw ExceptionInput("Comada invalida");
	}

	void serialize(ostream& out, bool isTypeRequired) const override {
		if (isTypeRequired) {
			SerializableType type = PreparatType;
			out.write(reinterpret_cast<const char*>(&type), sizeof(type));
		}
		size_t nameLength = nume.size();
		out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		out.write(nume.c_str(), nameLength);

		out.write(reinterpret_cast<const char*>(&pret), sizeof(pret));

		bool hasRecipe = (reteta != nullptr);
		out.write(reinterpret_cast<const char*>(&hasRecipe), sizeof(hasRecipe));
		if (hasRecipe) {
			reteta->serialize(out, false);
		}
	}

	void deserialize(istream& in) override {
		size_t nameLength;
		in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
		nume.resize(nameLength);
		in.read(&nume[0], nameLength);

		in.read(reinterpret_cast<char*>(&pret), sizeof(pret));

		bool hasRecipe;
		in.read(reinterpret_cast<char*>(&hasRecipe), sizeof(hasRecipe));
		if (hasRecipe) {
			reteta = new Reteta();
			reteta->deserialize(in);
		}
	}
};

class Meniu {
	string nume = "Meniu";
	vector<Preparat*> preparate;

public:
	Meniu() {}

	Meniu(const string& nume) {
		if (nume.length() > 3)
			this->nume = nume;
	}

	Meniu(const string& nume, const vector<Preparat*>& preparate) : nume(nume) {
		for (auto& preparat : preparate)
			this->preparate.push_back(new Preparat(*preparat));
	}

	Meniu(const Meniu& meniu) : nume(meniu.nume) {
		for (auto& preparat : meniu.preparate)
			this->preparate.push_back(new Preparat(*preparat));
	}

	Meniu& operator=(const Meniu& meniu) {
		if (this != &meniu) {
			this->nume = meniu.nume;
			for (auto& preparat : preparate)
				delete preparat;
			this->preparate.clear();
			for (auto& preparat : meniu.preparate)
				this->preparate.push_back(new Preparat(*preparat));
		}
		return *this;
	}

	~Meniu() {
		for (auto& preparat : this->preparate) {
			delete preparat;
			preparat = nullptr;
		}
	}

	void adaugaPreparat(Preparat* preparat) {
		for (auto& p : preparate) {
			if (p->getNume() == preparat->getNume())
				throw ExceptionInput("Preparat deja existent");
		}
		preparate.push_back(preparat);
	}

	void afiseazaPreparate(bool cuReteta) {
		if (preparate.empty()) {
			cout << "Nu sunt preparate" << endl;
			return;
		}

		int numar = 1;
		for (auto& preparat : preparate) {
			cout << numar++ << ". ";
			preparat->afiseazaPreparat(cuReteta);
		}
	}

	friend ostream& operator<<(ostream& out, Meniu& meniu) {
		out << "Numarul de preparate din meniu este: " << meniu.preparate.size() << endl;

		return out;
	}

	friend istream& operator>>(istream& in, Meniu& meniu) {
		cout << "Introduceti numele meniului: ";
		string nume;
		getline(in, nume);
		string numeTrim = Utils::trim(nume);
		if (numeTrim.length() > 3)
			meniu.nume = nume;

		return in;
	}

	int getNrPreparate() const {
		return this->preparate.size();
	}

	const string getNume() const {
		return this->nume;
	}

	void setNume(const string& nume) {
		if (nume.length() > 3)
			this->nume = nume;
	}

	void stergePreparat() {
		afiseazaPreparate(false);
		if (this->preparate.empty()) {
			cout << "Nu exista preparate in meniu." << endl;
			return;
		}

		cout << "0. Iesire" << endl;
		while (true) {
			cout << "Selecteaza preparatul: ";
			int optiune;
			cin >> optiune;
			if (cin.fail()) {
				cin.clear();
				cout << "Optiune invalida" << endl;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				continue;
			}

			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			if (optiune < 0 || static_cast<size_t>(optiune) > preparate.size()) {
				cout << "Optiune invalida" << endl;
				continue;
			}
			if (optiune == 0) {
				break;
			}
			int index = optiune - 1;

			delete preparate[index];
			preparate.erase(preparate.begin() + index);

			cout << "Preparatul a fost eliminat." << endl;
			break;
		}
	}

	void stergePreparat(const string& cmd) {
		if (this->preparate.empty()) {
			throw ExceptionInput("Numarul preparatelor este 0");
		}

		istringstream iss(cmd);
		string token, nume;
		if (getline(iss, token, ' ') && token == "STERGE" && getline(iss, token, ' ') && token == "PREPARAT") {
			if (!getline(iss, nume, ',')) {
				throw ExceptionInput("Nume invalid");
			}

			int index = -1;
			string trimName = Utils::trim(nume);
			for (auto it = this->preparate.begin(); it != this->preparate.end(); ++it) {
				if ((*it)->getNume() == trimName) {
					delete* it;
					this->preparate.erase(it);
					return;
				}
			}
			throw ExceptionInput("Preparat indexistent");
		}
	}

	Preparat* operator[](int pozitie) {
		if (pozitie >= 0 && static_cast<size_t>(pozitie) < preparate.size()) {
			return preparate[pozitie];
		}
		return nullptr;
	}


	void operator()(const string& nume, float pret) {
		for (auto& preparat : this->preparate) {
			if (preparat->getNume() == nume) {
				preparat->setPret(pret);
				return;
			}
		}
		throw ExceptionInput("Preparat inexistent");
	}


	void afiseasaMeniu(bool cuReteta) {
		cout << endl;
		cout << "Meniu cu preparate disponibile:" << endl;
		cout << "-------------------------------" << endl;
		if (this->preparate.empty()) {
			cout << "Meniul nu contine preparate." << endl;
		}
		afiseazaPreparate(cuReteta);
		cout << "-------------------------------" << endl;
	}
};

class Comanda : public Serializable {
	const int id;
	string client = "Client";
	vector<Preparat*> preparate;
	vector<int> cantitatePreparate;
	Meniu* meniu = nullptr;
	Restaurant* restaurant = nullptr;
	float costTotal = 0;
	static int numarComezi;

public:
	Comanda() : id(numarComezi++) {}

	Comanda(Meniu* meniu, Restaurant* restaurant) : meniu(meniu), restaurant(restaurant), id(numarComezi++) {}

	Comanda(const Comanda& comanda) : id(numarComezi++) {
		this->client = comanda.client;
		this->meniu = comanda.meniu;
		for (size_t i = 0; i < comanda.preparate.size(); i++) {
			this->preparate.push_back(new Preparat(*comanda.preparate[i]));
			this->cantitatePreparate.push_back(comanda.cantitatePreparate[i]);
		}
		this->costTotal = comanda.costTotal;
	}

	~Comanda() {
		for (auto& preparat : preparate) {
			delete preparat;
			preparat = nullptr;
		}
	}

	Comanda& operator=(const Comanda& comanda) {
		if (this != &comanda) {
			for (auto& preparat : this->preparate) {
				delete preparat;
			}
			this->preparate.clear();
			this->cantitatePreparate.clear();
			this->meniu = comanda.meniu;
			this->restaurant = comanda.restaurant;
			this->costTotal = comanda.costTotal;
			this->client = comanda.client;
			for (size_t i = 0; i < comanda.preparate.size(); i++) {
				this->preparate.push_back(new Preparat(*comanda.preparate[i]));
				this->cantitatePreparate.push_back(comanda.cantitatePreparate[i]);
			}
		}
		return *this;
	}

	friend istream& operator>>(istream& in, Comanda& comanda);

	friend ostream& operator<<(ostream& out, const Comanda& comanda) {
		out << endl;
		out << "Id: " << comanda.id << " | " << "Client: " << comanda.client << endl;
		out << "----------Preparate----------" << endl;
		for (size_t i = 0; i < comanda.preparate.size(); i++) {
			out << i + 1 << ". " << comanda.cantitatePreparate[i] << "x " << endl << *comanda.preparate[i];
		}
		out << "Cost total: " << comanda.costTotal << endl;

		return out;
	}

	static int getNumarComenzi() {
		return Comanda::numarComezi;
	}

	static void decreaseNumarComenzi() {
		if (Comanda::numarComezi > 0)
			Comanda::numarComezi--;
	}

	int getId() const {
		return this->id;
	}

	int getNrPreparate() const {
		return this->preparate.size();
	}

	int getCostTotal() const {
		return this->costTotal;
	}

	string getClient() const {
		return this->client;
	}

	void serialize(ostream& out, bool isTypeRequired) const override {
		if (isTypeRequired) {
			SerializableType type = ComandaType;
			out.write(reinterpret_cast<const char*>(&type), sizeof(type));
		}
		size_t clientLength = client.length();
		out.write(reinterpret_cast<const char*>(&clientLength), sizeof(clientLength));
		out.write(client.c_str(), clientLength);
		out.write(reinterpret_cast<const char*>(&costTotal), sizeof(costTotal));

		size_t numPreparate = preparate.size();
		out.write(reinterpret_cast<const char*>(&numPreparate), sizeof(numPreparate));

		for (size_t i = 0; i < numPreparate; ++i) {
			preparate[i]->serialize(out, false);
			out.write(reinterpret_cast<const char*>(&cantitatePreparate[i]), sizeof(cantitatePreparate[i]));
		}
	}

	void deserialize(istream& in) override {
		size_t clientLength;
		in.read(reinterpret_cast<char*>(&clientLength), sizeof(clientLength));
		client.resize(clientLength);
		in.read(&client[0], clientLength);

		in.read(reinterpret_cast<char*>(&costTotal), sizeof(costTotal));
		size_t numPreparate;
		in.read(reinterpret_cast<char*>(&numPreparate), sizeof(numPreparate));

		preparate.clear();
		cantitatePreparate.clear();
		for (size_t i = 0; i < numPreparate; ++i) {
			Preparat* p = new Preparat(this->restaurant);
			p->deserialize(in);
			preparate.push_back(p);

			int cantitate;
			in.read(reinterpret_cast<char*>(&cantitate), sizeof(cantitate));
			cantitatePreparate.push_back(cantitate);
		}
	}
};

int Comanda::numarComezi = 0;

class Restaurant {
	string nume = "Restaurant";
	vector<Reteta*> retete;
	vector<Comanda*> comenzi;
	Meniu* meniu = nullptr;
	map<string, int> stocIngrediente;

public:
	Restaurant() {}

	Restaurant(string nume) {
		if (nume.length() > 3)
			this->nume = nume;
	}

	Restaurant(const Restaurant& restaurant) {
		this->nume = restaurant.nume;
		this->meniu = restaurant.meniu;
		this->stocIngrediente = restaurant.stocIngrediente;
		for (auto& reteta : restaurant.retete) {
			this->retete.push_back(new Reteta(*reteta));
		}
		for (auto& comanda : restaurant.comenzi) {
			this->comenzi.push_back(new Comanda(*comanda));
		}
	}

	Restaurant& operator=(const Restaurant& restaurant) {
		if (this != &restaurant) {
			for (auto& reteta : this->retete)
				delete reteta;
			for (auto& comanda : this->comenzi)
				delete comanda;
			this->retete.clear();
			this->comenzi.clear();
			this->nume = restaurant.nume;
			this->meniu = restaurant.meniu;
			this->stocIngrediente = restaurant.stocIngrediente;

			for (size_t i = 0; i < restaurant.retete.size(); i++) {
				this->retete.push_back(new Reteta(*restaurant.retete[i]));
			}
			for (size_t i = 0; i < restaurant.comenzi.size(); i++) {
				this->comenzi.push_back(new Comanda(*restaurant.comenzi[i]));
			}
		}

		return *this;
	}

	~Restaurant() {
		for (auto& reteta : this->retete) {
			delete reteta;
			reteta = nullptr;
		}

		for (auto& comanda : this->comenzi) {
			delete comanda;
			comanda = nullptr;
		}
	}

	void setMeniu(Meniu* meniu) {
		if (meniu != nullptr)
			this->meniu = meniu;
	}

	void adaugaReteta(Reteta* reteta) {
		if (reteta != nullptr)
			this->retete.push_back(reteta);
	}

	void adaugaComanda(Comanda* comanda) {
		if (comanda != nullptr)
			this->comenzi.push_back(comanda);
	}

	void afiseasaRetete() {
		for (size_t i = 0; i < this->retete.size(); i++)
			cout << i + 1 << ". " << *this->retete[i];
	}

	int getNrRetete() {
		return this->retete.size();
	}

	int getNrComenzi() {
		return this->comenzi.size();
	}

	Reteta* getRetetaPrinIndex(int numarReteta) {
		int indexReteta = numarReteta - 1;
		if (indexReteta >= 0 && static_cast<size_t>(indexReteta) < this->retete.size()) {
			return this->retete[indexReteta];
		}
		return nullptr;
	}

	Reteta* getRetetaPrinNume(string nume) {
		for (int i = 0; i < this->retete.size(); i++) {
			if (this->retete[i]->getNume() == nume)
				return this->retete[i];
		}

		return nullptr;
	}

	Comanda* getComanda(int numarComanda) {
		if (numarComanda >= 0 && static_cast<size_t>(numarComanda) < this->retete.size()) {
			return this->comenzi[numarComanda];
		}
		return nullptr;
	}

	const map<string, int>& getStocIngrediente() const {
		return stocIngrediente;
	}

	void setStocIngrediente(const map<string, int>& nouStoc) {
		stocIngrediente = nouStoc;
	}

	void adaugaStoc(string ingredient, int cantitate) {
		string ingredintTrim = Utils::trim(ingredient);
		if (ingredintTrim.length() < 3) {
			cout << "Numele ingredientului trebuie sa aiba minim 3 caractere" << endl;
			return;
		}
		if (cantitate <= 0) {
			cout << "Cantitatea trebuie sa fie mai mare de 0";
			return;
		}

		this->stocIngrediente[ingredintTrim] += cantitate;
	}

	friend istream& operator>>(istream& in, Restaurant& restaurant) {
		while (true) {
			string ingredient;
			cout << "Introdu ingredientul: ";
			getline(in, ingredient);
			string ingredintTrim = Utils::trim(ingredient);
			if (ingredient.length() < 3) {
				cout << "Numele ingredientului trebuie sa aiba minim 3 caractere" << endl;
				continue;
			}
			cout << "Introdu cantitatea: ";
			int cantitate;
			in >> cantitate;
			if (in.fail()) {
				in.clear();
				cout << "Cantitate invalida" << endl;
				in.ignore(numeric_limits<streamsize>::max(), '\n');
				continue;
			}
			in.ignore(numeric_limits<streamsize>::max(), '\n');
			if (cantitate > 0) {
				restaurant.stocIngrediente[ingredintTrim] += cantitate;
				return in;
			}
			if (cantitate <= 0) cout << "Cantitatea trebuie sa fie mai mare de 0";
		}

		return in;
	}

	void operator()(const string& cmd) {
		istringstream iss(cmd);
		string id, token;
		if (getline(iss, token, ' ') && token == "AFISEAZA" && getline(iss, token, ' ') && token == "COMANDA") {
			if (!getline(iss, id, ' ')) {
				for (int i = 0; i < Comanda::getNumarComenzi(); i++) {
					cout << *this->comenzi[i];
				}
			}
			else {
				int idInt;
				try {
					idInt = stof(id);
				}
				catch (const invalid_argument&) {
					throw ExceptionInput("Id invalid");
				}
				bool found = false;
				for (int i = 0; i < Comanda::getNumarComenzi(); i++) {
					if (this->comenzi[i]->getId() == idInt) {
						cout << *this->comenzi[i];
						found = true;
						break;
					}
				}
				if (!found) throw ExceptionInput("Id inexistent");
			}
		}
	}

	void scadeStoc(const string& ingredient, int cantitate) {
		if (stocIngrediente.find(ingredient) != stocIngrediente.end() && stocIngrediente[ingredient] >= cantitate) {
			stocIngrediente[ingredient] -= cantitate;
			return;
		}
		if (stocIngrediente.find(ingredient) != stocIngrediente.end()) throw ExceptionInput("Ingredient indisponibil");

		throw ExceptionInput("Cantitatea este prea mare. Stoc actual: " + to_string(stocIngrediente[ingredient]));
	}

	void scadeStoc() {
		string ingredient;
		cout << "Introdu ingredientul: ";
		getline(cin, ingredient);
		string ingredientTrim = Utils::trim(ingredient);
		if (stocIngrediente.find(ingredientTrim) == stocIngrediente.end())
			throw ExceptionInput("Ingredient indisponibil");
		cout << "Introdu cantitatea: ";
		int cantitate;
		cin >> cantitate;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			throw ExceptionInput("Cantitate invalida");
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		if (cantitate < 0)
			throw ExceptionInput("Cantitatea trebuie sa fie pozitiva");

		if (stocIngrediente[ingredientTrim] < cantitate)
			throw ExceptionInput("Cantitatea este prea mare. Stoc actual: " + to_string(stocIngrediente[ingredientTrim]));

		stocIngrediente[ingredientTrim] -= cantitate;
	}

	friend ostream& operator<<(ostream& out, Restaurant& restaurant) {
		if (restaurant.stocIngrediente.empty()) {
			out << "Stocul este gol." << endl;
		}
		else {
			map<string, int>::iterator iterator = restaurant.stocIngrediente.begin();
			while (iterator != restaurant.stocIngrediente.end())
			{
				out << "Ingredient: " << iterator->first << " | Cantitate: " << iterator->second << endl;
				iterator++;
			}
		}

		return out;
	}

	void modificaReteta() {
		if (this->retete.empty())
			throw ExceptionInput("0 retete inregistrate");
		afiseasaRetete();
		cout << "Selecteaza reteta: ";
		int numarReteta;
		cin >> numarReteta;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			throw ExceptionInput("Valoare invalida");
		}
		if (static_cast<size_t>(numarReteta) > this->retete.size()) {
			throw ExceptionInput("Reteta inexistenta");
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		Reteta* reteta = getRetetaPrinIndex(numarReteta);
		cout << *reteta;
		cout << "Intro ingredientu: ";
		string ingredient;
		getline(cin, ingredient);
		string ingredienTrim = Utils::trim(ingredient);
		if (ingredienTrim.length() < 3)
			throw ExceptionInput("Numele ingredientului trebuie sa aiba minim 3 caractere");

		cout << "Introdu cantitatea: ";
		int cantitate;
		cin >> cantitate;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			throw ExceptionInput("Valoare invalida");
		}
		if (cantitate <= 0) {
			throw ExceptionInput("Cantitatea trebuie ssa fie pozitiva");
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		reteta->modificaReteta(ingredienTrim, cantitate);
	}
};

Preparat::Preparat(Restaurant* restaurant) : restaurant(restaurant) {}

Preparat::Preparat(const string& nume, float pret, Restaurant* restaurant) : restaurant(restaurant) {
	if (nume.length() > 2)
		this->nume = nume;
	if (pret > 0 && pret < Preparat::pretMaxim)
		this->pret = pret;
}

istream& operator>>(istream& in, Preparat& p) {
	if (p.restaurant->getNrRetete() == 0) throw ExceptionInput("Nu exista retete.");
	p.restaurant->afiseasaRetete();
	cout << "Alege numarul retetei: ";
	while (true) {
		int nrReteta;
		in >> nrReteta;
		if (in.fail()) {
			in.clear();
			cout << "Valoare invalida." << endl;
			in.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}
		in.ignore(numeric_limits<streamsize>::max(), '\n');
		if (nrReteta > p.restaurant->getNrRetete()) {
			cout << "Numarul retetei este prea mare" << endl;
		}
		else {
			p.reteta = new Reteta(*p.restaurant->getRetetaPrinIndex(nrReteta));
			break;
		}
	}

	return in;
}

ifstream& operator>>(ifstream& in, Preparat& preparat) {
	string line;
	getline(in, line);
	if (line.size() == 0) return in;
	stringstream ss(line);

	getline(ss, preparat.nume, ',');
	preparat.nume = Utils::trim(preparat.nume);

	string pret;
	getline(ss, pret, ',');
	try {
		preparat.pret = stof(pret);
	}
	catch (const invalid_argument&) {
		throw ExceptionInput("Preparatul " + preparat.nume + "are pretul invalid.");
	}

	string numeleRetetei;
	getline(ss, numeleRetetei);
	Reteta* reteta = preparat.restaurant->getRetetaPrinNume(Utils::trim(numeleRetetei));
	if (reteta == nullptr) throw ExceptionInput("Preparatul " + preparat.nume + "are o reteta inexistenta.");
	preparat.reteta = new Reteta(*reteta);

	return in;
}

istream& operator>>(istream& in, Comanda& comanda) {
	if (comanda.meniu->getNrPreparate() == 0) {
		throw ExceptionInput("Preparate indisponibile");
	}
	while (true) {
		cout << "Introduceti numele clientului: ";
		string nume;
		getline(in, nume);
		string numeTrim = Utils::trim(nume);
		if (numeTrim.length() > 2) {
			comanda.client = numeTrim;
			break;
		}
		else {
			cout << "Numele clientului trebuie sa fie mai lung de 2." << endl;
		}
	}
	int numarPreparate;
	while (true) {
		cout << "Introduceti numarul de preparate (maxim 30): ";
		in >> numarPreparate;

		if (in.fail()) {
			in.clear();
			cout << "Valoare invalida. Introduceti un numar." << endl;
			in.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}

		in.ignore(numeric_limits<streamsize>::max(), '\n');
		if (numarPreparate <= 0 || numarPreparate > 30) {
			cout << "Numarul de preparate trebuie sa fie intre 1 si 30." << endl;
		}
		else break;
	}
	comanda.meniu->afiseasaMeniu(false);
	comanda.preparate.clear();
	comanda.cantitatePreparate.clear();
	comanda.costTotal = 0;
	map<string, int> stocSiIngrediente = comanda.restaurant->getStocIngrediente();

	for (int i = 0; i < numarPreparate; i++) {
		int nrPreparat;
		Preparat* preparat = nullptr;
		cout << "Introduceti numarul preparatului: ";
		in >> nrPreparat;
		if (in.fail()) {
			in.clear();
			--i;
			cout << "Valoare invalida." << endl;
			in.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}
		else if (nrPreparat <= 0 || nrPreparat > comanda.meniu->getNrPreparate()) {
			--i;
			cout << "Preparat inexistent." << endl;
			in.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}
		else {
			preparat = (*comanda.meniu)[nrPreparat - 1];
		}
		in.ignore(numeric_limits<streamsize>::max(), '\n');

		int cantitate;
		cout << "Introduceti numarul portilor: ";
		in >> cantitate;
		if (in.fail()) {
			in.clear();
			--i;
			cout << "Valoare invalida." << endl;
		}
		else if (cantitate <= 0) {
			--i;
			cout << "Cantitatea trebuie sa fie pozitiva." << endl;
		}
		else {
			comanda.preparate.push_back(new Preparat(*preparat));
			comanda.cantitatePreparate.push_back(cantitate);
			const Reteta* reteta = preparat->getReteta();
			map<string, int> ingredienteSiCantitate = reteta->getIngredienteSiCantitate();

			for (const auto& ingredientCantitate : ingredienteSiCantitate) {
				const string& ingredient = ingredientCantitate.first;
				int cantitateNecesara = ingredientCantitate.second * cantitate;
				auto it = stocSiIngrediente.find(ingredient);
				if (it == stocSiIngrediente.end() || it->second < cantitateNecesara) {
					in.ignore(numeric_limits<streamsize>::max(), '\n');
					throw ExceptionInput("Ingredientul " + ingredient + " nu este disponibil in cantitatea necesara.");
				}
			}

			for (const auto& ingredientCantitate : ingredienteSiCantitate) {
				const string& ingredient = ingredientCantitate.first;
				int cantitateNecesara = ingredientCantitate.second * cantitate;
				try {
					if (stocSiIngrediente.find(ingredient) != stocSiIngrediente.end() && stocSiIngrediente[ingredient] >= cantitateNecesara) {
						stocSiIngrediente[ingredient] -= cantitateNecesara;
						continue;
					}
					if (stocSiIngrediente.find(ingredient) != stocSiIngrediente.end()) throw ExceptionInput("Ingredient indisponibil");

					throw ExceptionInput("Cantitatea este prea mare. Stoc actual: " + to_string(stocSiIngrediente[ingredient]));
				}
				catch (const ExceptionInput& e) {
					in.ignore(numeric_limits<streamsize>::max(), '\n');
					throw ExceptionInput("Stocul nu a putut fi scazut");
				}
			}
			comanda.costTotal = comanda.costTotal + cantitate * preparat->getPret();
		}
		in.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	comanda.restaurant->setStocIngrediente(stocSiIngrediente);
	comanda.restaurant->adaugaComanda(&comanda);

	return in;
}

class ColectieSerializable {
	vector<Serializable*> obiecte;
public:
	void clearObiecte() {
		obiecte.clear();
	}

	void adaugaObiect(Serializable* obj) {
		obiecte.push_back(obj);
	}

	void serializeToate(ostream& out) {
		for (auto& obj : obiecte) {
			obj->serialize(out, true);
		}
	}

	void deserializeToate(istream& in, Restaurant& restaurant, Meniu& meniu) {
		size_t numarObiecte;
		in.read(reinterpret_cast<char*>(&numarObiecte), sizeof(numarObiecte));
		for (size_t i = 0; i < numarObiecte; ++i) {
			SerializableType type;
			in.read(reinterpret_cast<char*>(&type), sizeof(type));

			Serializable* obj = nullptr;
			switch (type) {
			case RetetaType:
				obj = new Reteta();
				break;
			case PreparatType:
				obj = new Preparat(&restaurant);
				break;
			case ComandaType:
				obj = new Comanda(&meniu, &restaurant);
				break;
			}

			if (obj) {
				obj->deserialize(in);
				switch (type) {
				case RetetaType:
					restaurant.adaugaReteta((Reteta*)obj);
					break;
				case PreparatType:
					meniu.adaugaPreparat((Preparat*)obj);
					break;
				case ComandaType:
					restaurant.adaugaComanda((Comanda*)obj);
					break;
				}
				obiecte.push_back(obj);
			}
		}
	}
};

class ComandaIstoric {
public:
	string comanda;
	chrono::system_clock::time_point timestamp;

	ComandaIstoric(const string& comanda) : comanda(comanda), timestamp(chrono::system_clock::now()) {}
};

class RaportGenerator {
	list<ComandaIstoric> istoric;

public:
	void adaugaComanda(const string& comanda) {
		istoric.push_back(comanda);
	}

	void stergeUltimaComanda() {
		if (!istoric.empty()) {
			istoric.pop_back();
		}
	}

	void afiseazaIstoric() const {
		for (const auto& comanda : istoric) {
			time_t timp = chrono::system_clock::to_time_t(comanda.timestamp);
			char buffer[30];
			ctime_s(buffer, sizeof(buffer), &timp);
			cout << "Comanda: " << comanda.comanda << ", Timestamp: " << buffer;
		}
	}

	static void salveazaTimpInAplicatie(const chrono::system_clock::time_point& startTime) {
		auto endTime = chrono::system_clock::now();
		auto durata = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
		ofstream outFile("timp.txt", ios::app);
		if (outFile.is_open()) {
			outFile << durata << endl;
			outFile.close();
		}
		else {
			cout << "Nu am putut deschide fisierul pentru scriere: timp.txt" << endl;
		}
	}

	static void calculeazaTimpMediuPetrecut() {
		ifstream inFile("timp.txt");
		if (!inFile.is_open()) {
			cout << "Nu am putut deschide fisierul pentru citire: timp.txt" << endl;
			return;
		}

		int timp, suma = 0, numarSesiuni = 0;
		while (inFile >> timp) {
			suma += timp;
			++numarSesiuni;
		}

		inFile.close();

		if (numarSesiuni == 0) {
			cout << "Nu exista date inregistrate pentru a calcula timpul mediu." << endl;
			return;
		}

		double timpMediu = static_cast<double>(suma) / numarSesiuni;
		cout << "Timpul mediu petrecut in aplicatie este: " << timpMediu << " secunde." << endl;
	}

	static void genereazaRaportComenzi(Restaurant& restaurant, ostream& out) {
		if (restaurant.getNrComenzi() == 0) {
			out << "Nu exista comenzi pentru generarea raportului.\n";
			return;
		}

		float sumaTotala = 0.0f;
		float pretMinim = FLT_MAX;
		float pretMaxim = FLT_MIN;

		for (int i = 0; i < restaurant.getNrComenzi(); i++) {
			float costComanda = restaurant.getComanda(i)->getCostTotal();
			sumaTotala += costComanda;
			if (costComanda < pretMinim) pretMinim = costComanda;
			if (costComanda > pretMaxim) pretMaxim = costComanda;
		}

		float pretMediu = sumaTotala / restaurant.getNrComenzi();

		out << "Pretul mediu al unei comenzi este: " << pretMediu << endl;
		out << "Pretul minim al unei comenzi este: " << pretMinim << endl;
		out << "Pretul maxim al unei comenzi este: " << pretMaxim << endl;
	}

	static void genereazaRaportRetete(Restaurant& restaurant, ostream& out) {
		if (restaurant.getNrRetete() == 0) {
			out << "Nu exista retete pentru generarea raportului.\n";
			return;
		}

		float gramajTotal = 0.0f;
		int gramajMinim = INT_MAX;
		int gramajMaxim = INT_MIN;
		int numarRetete = 0;

		for (int i = 0; i < restaurant.getNrRetete(); i++) {
			int gramajReteta = 0;
			for (const auto& pereche : restaurant.getRetetaPrinIndex(i + 1)->getIngredienteSiCantitate()) {
				gramajReteta += pereche.second;
			}
			gramajTotal += gramajReteta;
			if (gramajReteta < gramajMinim) gramajMinim = gramajReteta;
			if (gramajReteta > gramajMaxim) gramajMaxim = gramajReteta;
		}

		float gramajMediu = gramajTotal / restaurant.getNrRetete();

		out << "Gramajul mediu al retetelor este: " << gramajMediu << " grame\n";
		out << "Gramajul minim al retetelor este: " << gramajMinim << " grame\n";
		out << "Gramajul maxim al retetelor este: " << gramajMaxim << " grame\n";
	}

	static void genereazaRaportUtilizatori(Restaurant& restaurant, ostream& out) {
		size_t numarComenzi = restaurant.getNrComenzi();
		set<string> numeUnice;

		for (size_t i = 0; i < numarComenzi; ++i) {
			Comanda* comanda = restaurant.getComanda(i);
			if (comanda) {
				numeUnice.insert(comanda->getClient());
			}
		}

		out << "Numarul total de utilizatori: " << numeUnice.size() << "\n";
		out << "Numele utilizatorilor:\n";
		for (const auto& nume : numeUnice) {
			out << nume << "\n";
		}
	}

	static void salveazaRaportInFisier(const string& numeFisier, const string& numeFunctie, Restaurant& restaurant) {
		ofstream fisier(numeFisier);
		if (fisier.is_open()) {
			if (numeFunctie == "comenzi") {
				RaportGenerator::genereazaRaportComenzi(restaurant, fisier);
			}
			else if (numeFunctie == "retete") {
				RaportGenerator::genereazaRaportRetete(restaurant, fisier);
			}
			else if (numeFunctie == "utilizatori") {
				RaportGenerator::genereazaRaportUtilizatori(restaurant, fisier);
			}
			fisier.close();
		}
		else {
			cout << "Nu am putut deschide fisierul pentru scriere: " << numeFisier << endl;
		}
	}
};


class Parser {
public:
	static void parseFiles(int argc, char* argv[], Restaurant& restaurant, Meniu& meniu, ColectieSerializable& colectie) {
		if (argc == 1) {
			ifstream inFile("data.bin", ios::binary);
			if (!inFile) {
				cerr << "Eroare la deschiderea fisierului binar pentru citire." << endl;
			}
			else {
				colectie.deserializeToate(inFile, restaurant, meniu);

				size_t stocSize;
				inFile.read(reinterpret_cast<char*>(&stocSize), sizeof(stocSize));
				for (size_t i = 0; i < stocSize; ++i) {
					size_t length;
					inFile.read(reinterpret_cast<char*>(&length), sizeof(length));
					string ingredient(length, ' ');
					inFile.read(&ingredient[0], length);

					int cantitate;
					inFile.read(reinterpret_cast<char*>(&cantitate), sizeof(cantitate));
					restaurant.adaugaStoc(ingredient, cantitate);
				}
			}
			inFile.close();
			return;
		}

		bool foundReteteFile = false;
		for (int i = 1; i < argc; ++i) {
			string filename = argv[i];
			ifstream file(filename);
			if (!file.is_open()) {
				cout << "Nu a putut fi deschis fisierul: " << filename << endl;
				continue;
			}

			if (filename == "retete.txt") {
				Reteta reteta;
				foundReteteFile = true;
				while (file >> reteta) {
					restaurant.adaugaReteta(new Reteta(reteta));
				}
			}
			else if (filename == "preparate.txt" && foundReteteFile) {
				try {
					Preparat preparat(&restaurant);
					while (file >> preparat) {
						if (preparat.getReteta() != nullptr)
							meniu.adaugaPreparat(new Preparat(preparat));
					}
				}
				catch (const ExceptionInput& e) {
					cout << e.what() << endl;
				}
			}

			file.close();
		}
	}

	static void saveData(Restaurant& restaurant, Meniu& meniu, ColectieSerializable& colectie) {
		ofstream outFile("data.bin", ios::binary);
		if (!outFile) {
			cerr << "Eroare la deschiderea fisierului pentru scriere." << endl;
			return;
		}
		size_t numarObiecte = restaurant.getNrRetete() + meniu.getNrPreparate() + restaurant.getNrComenzi();
		outFile.write(reinterpret_cast<const char*>(&numarObiecte), sizeof(numarObiecte));
		colectie.clearObiecte();
		for (size_t i = 0; i < restaurant.getNrRetete(); ++i) {
			colectie.adaugaObiect(restaurant.getRetetaPrinIndex(i + 1));
		}

		for (size_t i = 0; i < meniu.getNrPreparate(); ++i) {
			colectie.adaugaObiect(meniu[i]);
		}

		for (size_t i = 0; i < restaurant.getNrComenzi(); ++i) {
			colectie.adaugaObiect(restaurant.getComanda(i));
		}

		colectie.serializeToate(outFile);

		const auto& stocIngrediente = restaurant.getStocIngrediente();
		size_t stocSize = stocIngrediente.size();
		outFile.write(reinterpret_cast<const char*>(&stocSize), sizeof(stocSize));

		for (const auto& pereche : stocIngrediente) {
			size_t length = pereche.first.size();
			outFile.write(reinterpret_cast<const char*>(&length), sizeof(length));
			outFile.write(pereche.first.c_str(), length);
			outFile.write(reinterpret_cast<const char*>(&pereche.second), sizeof(pereche.second));
		}

		cout << "Datele au fost salvate" << endl;

		outFile.close();
	}

	static void incarcaCSVRetete(Restaurant& restaurant, string& cmd) {
		stringstream ss(cmd);
		string token;
		string numeFisierCSV;

		getline(ss, token, ' ');
		getline(ss, token, ' ');
		if (!getline(ss, numeFisierCSV)) {
			cout << "Nume fisier CSV nu a fost specificat in comanda." << endl;
			return;
		}

		ifstream file(numeFisierCSV);

		if (!file.is_open()) {
			cout << "Nu s-a putut deschide fisierul: " << numeFisierCSV << endl;
			return;
		}

		string linie;
		while (getline(file, linie)) {
			stringstream linieStream(linie);
			string numeReteta;
			getline(linieStream, numeReteta, ',');

			Reteta* reteta = new Reteta(numeReteta);
			string ingredient, cantitateStr;
			int cantitate;

			while (getline(linieStream, ingredient, ',') && getline(linieStream, cantitateStr, ',')) {
				try {
					cantitate = stoi(cantitateStr);
					if (cantitate > 0) {
						reteta->modificaReteta(ingredient, cantitate);
					}
					else {
						cout << "Cantitate invalida pentru ingredientul '" << ingredient << "' in reteta '" << numeReteta << "'" << endl;
					}
				}
				catch (const invalid_argument&) {
					cout << "Cantitate invalida pentru ingredientul '" << ingredient << "' in reteta '" << numeReteta << "'" << endl;
				}
			}

			restaurant.adaugaReteta(reteta);
		}

		file.close();
		cout << "Fisierul CSV a fost incarcat cu succes" << endl;
	}
};

int main(int arg, char* argv[])
{
	auto startTime = chrono::system_clock::now();
	Restaurant restaurant("Restaurant");
	Meniu meniu;
	ColectieSerializable colectie;
	RaportGenerator raportGenerator;
	restaurant.setMeniu(&meniu);
	string cmd;
	Parser::parseFiles(arg, argv, restaurant, meniu, colectie);

	do {
		cout << endl;
		cout << "------------COMENZI------------" << endl;
		cout << "AFISEAZA MENIU" << endl;
		cout << "ADAUGA PREPARAT [nume], [pret]" << endl;
		cout << "ADAUGA STOC" << endl;
		cout << "VERIFICA STOC" << endl;
		cout << "STERGE PREPARAT [nume]," << endl;
		cout << "ADAUGA RETETA" << endl;
		cout << "PLASEAZA COMANDA" << endl;
		cout << "SCADE STOC" << endl;
		cout << "AFISEAZA COMANDA [id]" << endl;
		cout << "MODIFICA RETETA" << endl;
		cout << "MODIFICA PRET PREPARAT [nume], [pret]" << endl;
		cout << "AFISEAZA RETETE" << endl;
		cout << "RAPORT COMENZI" << endl;
		cout << "RAPORT RETETE" << endl;
		cout << "RAPORT UTILIZATORI" << endl;
		cout << "RAPORT CMD" << endl;
		cout << "INCARCA CSV [nume fisier]" << endl;
		cout << "SAVE" << endl;
		cout << "EXIT" << endl;
		cout << "-------------------------------" << endl;

		cout << "Introdu o comanda: ";
		getline(cin, cmd);
		string cmdTrim = Utils::trim(cmd);
		raportGenerator.adaugaComanda(cmdTrim);
		if (cmdTrim == "AFISEAZA MENIU") {
			meniu.afiseasaMeniu(true);
		}
		else if (Utils::startsWith(cmdTrim, "ADAUGA PREPARAT")) {
			try {
				string nume;
				float pret;
				Preparat::proceseazaPreparat(cmdTrim, nume, pret);
				Preparat* preparat = new Preparat(nume.data(), pret, &restaurant);
				try {
					cin >> *preparat;
					meniu.adaugaPreparat(preparat);
				}
				catch (const ExceptionInput& e) {
					cout << e.what() << endl;
					delete preparat;
				}
			}
			catch (const ExceptionInput& e) {
				cout << e.what() << endl;
			}
		}
		else if (Utils::startsWith(cmdTrim, "ADAUGA STOC")) {
			cin >> restaurant;
		}
		else if (cmdTrim == "VERIFICA STOC") {
			cout << restaurant;
		}
		else if (Utils::startsWith(cmdTrim, "STERGE PREPARAT")) {
			try {
				meniu.stergePreparat(cmdTrim);
			}
			catch (const ExceptionInput& e) {
				cout << e.what() << endl;
			}
		}
		else if (Utils::startsWith(cmdTrim, "ADAUGA RETETA")) {
			Reteta* reteta = new Reteta();
			cin >> *reteta;
			restaurant.adaugaReteta(reteta);
		}
		else if (Utils::startsWith(cmdTrim, "PLASEAZA COMANDA")) {
			Comanda* comanda = new Comanda(&meniu, &restaurant);
			try {
				cin >> *comanda;
			}
			catch (const ExceptionInput& e) {
				delete comanda;
				Comanda::decreaseNumarComenzi();
				cout << e.what() << endl;
			}
		}
		else if (Utils::startsWith(cmdTrim, "SCADE STOC")) {
			try {
				restaurant.scadeStoc();
			}
			catch (const ExceptionInput& e) {
				cout << e.what() << endl;
			}
		}
		else if (Utils::startsWith(cmdTrim, "AFISEAZA COMANDA")) {
			try {
				restaurant(cmdTrim);
			}
			catch (const ExceptionInput& e) {
				cout << e.what() << endl;
			}
		}
		else if (Utils::startsWith(cmdTrim, "MODIFICA RETETA")) {
			try {
				restaurant.modificaReteta();
			}
			catch (const ExceptionInput& e) {
				cout << e.what() << endl;
			}
		}
		else if (Utils::startsWith(cmdTrim, "MODIFICA PRET PREPARAT")) {
			try {
				string nume;
				float pret;
				Preparat::proceseazaPreparat(cmdTrim, nume, pret);
				meniu(nume, pret);
			}
			catch (const ExceptionInput& e) {
				cout << e.what() << endl;
			}
		}
		else if (cmdTrim == "AFISEAZA RETETE") {
			cout << "-------Retete-------" << endl;
			if (restaurant.getNrRetete() == 0)
				cout << "0 retete inregistrate" << endl;
			else
				restaurant.afiseasaRetete();
		}
		else if (cmdTrim == "SAVE") {
			Parser::saveData(restaurant, meniu, colectie);
		}
		else if (cmdTrim == "RAPORT COMENZI") {
			RaportGenerator::genereazaRaportComenzi(restaurant, cout);
			RaportGenerator::salveazaRaportInFisier("raport_comenzi.txt", "comenzi", restaurant);
		}
		else if (cmdTrim == "RAPORT RETETE") {
			RaportGenerator::genereazaRaportRetete(restaurant, cout);
			RaportGenerator::salveazaRaportInFisier("raport_retete.txt", "retete", restaurant);
		}
		else if (cmdTrim == "RAPORT UTILIZATORI") {
			RaportGenerator::genereazaRaportUtilizatori(restaurant, cout);
			RaportGenerator::salveazaRaportInFisier("raport_utilizatori.txt", "utilizatori", restaurant);
		}
		else if (cmdTrim == "RAPORT CMD") {
			raportGenerator.afiseazaIstoric();
		}
		else if (cmdTrim == "TIMP MEDIU") {
			RaportGenerator::calculeazaTimpMediuPetrecut();
		}
		else if (Utils::startsWith(cmdTrim, "INCARCA CSV")) {
			Parser::incarcaCSVRetete(restaurant, cmdTrim);
		}
		else if (cmdTrim == "EXIT") {
			cout << "Iesire." << endl;
		}
		else {
			raportGenerator.stergeUltimaComanda();
			cout << "Comanda invalida.";
		}
	} while (cmd != "EXIT");

	RaportGenerator::salveazaTimpInAplicatie(startTime);
	return 0;
}
