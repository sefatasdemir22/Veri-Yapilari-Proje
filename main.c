
//23120205078 Sefa Tasdemir

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_SIZE 5

// Yapılar
typedef struct Kargo {
    int gonderiID;
    char tarih[20];
    char teslimDurumu[50];
    int teslimSuresi;
    struct Kargo *next;
} Kargo;

typedef struct Stack {
    Kargo *items[MAX_STACK_SIZE];
    int top;
} Stack;

typedef struct Musteri {
    int musteriID;
    char isim[50];
    char soyisim[50];
    Stack gonderimGecmisi;
    struct Musteri *next;
} Musteri;

typedef struct PriorityQueue {
    Kargo *kargolar[100];
    int size;
} PriorityQueue;

typedef struct TreeNode {
    char cityName[50];
    int cityID;
    struct TreeNode *children[10];
    int childCount;
} TreeNode;

// Global Değişkenler
Musteri *musteriListesi = NULL;
PriorityQueue pq = { .size = 0 };
TreeNode *root = NULL;

// Fonksiyon Prototipleri
void teslimEdilmisKargolariSiralama(Kargo **kargolar, int n);
int binarySearchKargo(Kargo **kargolar, int n, int gonderiID);
void teslimEdilmisKargolariListeleVeAra();
void musteriEkle(int musteriID, char *isim, char *soyisim);
void kargoEkle(int musteriID, int gonderiID, char *tarih, char *teslimDurumu, int teslimSuresi);
void gonderimGecmisiSorgula(int musteriID);
void priorityQueueEkle(PriorityQueue *pq, Kargo *kargo);
void priorityQueueIsle(PriorityQueue *pq);
void teslimEdilmemisleriSirala();
TreeNode* createTreeNode(int cityID, char *cityName);
void addCity(TreeNode *parent, int cityID, char *cityName);
void printTree(TreeNode *node, int depth);
int calculateTreeDepth(TreeNode *node);
int calculateShortestDeliveryTime(TreeNode *node, int currentDepth);

// Teslim edilmiş kargoları sıralama
void teslimEdilmisKargolariSiralama(Kargo **kargolar, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (kargolar[j]->gonderiID > kargolar[j + 1]->gonderiID) {
                Kargo *temp = kargolar[j];
                kargolar[j] = kargolar[j + 1];
                kargolar[j + 1] = temp;
            }
        }
    }
}

// Binary Search fonksiyonu
int binarySearchKargo(Kargo **kargolar, int n, int gonderiID) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (kargolar[mid]->gonderiID == gonderiID) {
            return mid;
        } else if (kargolar[mid]->gonderiID < gonderiID) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1; // Bulunamadı
}

// Teslim Edilmiş Kargoları Listele ve Ara
void teslimEdilmisKargolariListeleVeAra() {
    Kargo *teslimEdilmis[100];
    int count = 0;

    Musteri *musteri = musteriListesi;
    while (musteri != NULL) {
        for (int i = 0; i < musteri->gonderimGecmisi.top; i++) {
            if (strcmp(musteri->gonderimGecmisi.items[i]->teslimDurumu, "Teslim Edildi") == 0) {
                teslimEdilmis[count++] = musteri->gonderimGecmisi.items[i];
            }
        }
        musteri = musteri->next;
    }

    teslimEdilmisKargolariSiralama(teslimEdilmis, count);

    printf("\nTeslim Edilmiş Kargolar:\n");
    for (int i = 0; i < count; i++) {
        printf("- Kargo ID: %d, Tarih: %s, Durum: %s, Süre: %d\n",
               teslimEdilmis[i]->gonderiID, teslimEdilmis[i]->tarih,
               teslimEdilmis[i]->teslimDurumu, teslimEdilmis[i]->teslimSuresi);
    }

    int arananID;
    printf("\nAramak istediğiniz Kargo ID'sini girin: ");
    scanf("%d", &arananID);

    int index = binarySearchKargo(teslimEdilmis, count, arananID);
    if (index != -1) {
        printf("Kargo bulundu: ID: %d, Tarih: %s, Durum: %s, Süre: %d\n",
               teslimEdilmis[index]->gonderiID, teslimEdilmis[index]->tarih,
               teslimEdilmis[index]->teslimDurumu, teslimEdilmis[index]->teslimSuresi);
    } else {
        printf("Kargo bulunamadı!\n");
    }
}

// Müşteri Ekle
void musteriEkle(int musteriID, char *isim, char *soyisim) {
    Musteri *yeniMusteri = (Musteri *)malloc(sizeof(Musteri));
    yeniMusteri->musteriID = musteriID;
    strcpy(yeniMusteri->isim, isim);
    strcpy(yeniMusteri->soyisim, soyisim);
    yeniMusteri->gonderimGecmisi.top = 0;
    yeniMusteri->next = musteriListesi;
    musteriListesi = yeniMusteri;
    printf("Müşteri eklendi: %d - %s %s\n", musteriID, isim, soyisim);
}

// Kargo Ekle
void kargoEkle(int musteriID, int gonderiID, char *tarih, char *teslimDurumu, int teslimSuresi) {
    Musteri *musteri = musteriListesi;
    while (musteri != NULL && musteri->musteriID != musteriID) {
        musteri = musteri->next;
    }

    if (musteri == NULL) {
        printf("Müşteri bulunamadı!\n");
        return;
    }

    Kargo *yeniKargo = (Kargo *)malloc(sizeof(Kargo));
    yeniKargo->gonderiID = gonderiID;
    strcpy(yeniKargo->tarih, tarih);
    strcpy(yeniKargo->teslimDurumu, teslimDurumu);
    yeniKargo->teslimSuresi = teslimSuresi;
    yeniKargo->next = NULL;

    if (musteri->gonderimGecmisi.top >= MAX_STACK_SIZE) {
        for (int i = 1; i < MAX_STACK_SIZE; i++) {
            musteri->gonderimGecmisi.items[i - 1] = musteri->gonderimGecmisi.items[i];
        }
        musteri->gonderimGecmisi.top--;
    }
    musteri->gonderimGecmisi.items[musteri->gonderimGecmisi.top++] = yeniKargo;

    if (strcmp(teslimDurumu, "Teslim Edilmedi") == 0) {
        priorityQueueEkle(&pq, yeniKargo);
    }

    printf("Kargo eklendi: ID: %d, Tarih: %s, Durum: %s, Süre: %d\n",
           gonderiID, tarih, teslimDurumu, teslimSuresi);
}

// Gönderim Geçmişi Sorgula
void gonderimGecmisiSorgula(int musteriID) {
    Musteri *musteri = musteriListesi;
    while (musteri != NULL && musteri->musteriID != musteriID) {
        musteri = musteri->next;
    }

    if (musteri == NULL) {
        printf("Müşteri bulunamadı!\n");
        return;
    }

    if (musteri->gonderimGecmisi.top == 0) {
        printf("Gönderim geçmişi boş!\n");
        return;
    }

    printf("Son Gönderim Geçmişi (En son gönderilen):\n");
    for (int i = musteri->gonderimGecmisi.top - 1; i >= 0; i--) {
        Kargo *kargo = musteri->gonderimGecmisi.items[i];
        printf("- Kargo ID: %d, Tarih: %s, Durum: %s, Süre: %d\n",
               kargo->gonderiID, kargo->tarih, kargo->teslimDurumu, kargo->teslimSuresi);
    }
}

// Priority Queue İşlemleri
void priorityQueueEkle(PriorityQueue *pq, Kargo *kargo) {
    int i = pq->size - 1;
    while (i >= 0 && pq->kargolar[i]->teslimSuresi > kargo->teslimSuresi) {
        pq->kargolar[i + 1] = pq->kargolar[i];
        i--;
    }
    pq->kargolar[i + 1] = kargo;
    pq->size++;
}

void priorityQueueIsle(PriorityQueue *pq) {
    if (pq->size == 0) {
        printf("Queue boş!\n");
        return;
    }

    Kargo *kargo = pq->kargolar[0];
    printf("İşlenen Kargo: ID: %d, Teslim Süresi: %d, Durum: %s\n",
           kargo->gonderiID, kargo->teslimSuresi, kargo->teslimDurumu);

    for (int i = 1; i < pq->size; i++) {
        pq->kargolar[i - 1] = pq->kargolar[i];
    }
    pq->size--;
}

// Teslim Edilmemişleri Sırala
void teslimEdilmemisleriSirala() {
    if (pq.size == 0) {
        printf("Teslim edilmemiş kargo bulunmamaktadır.\n");
        return;
    }

    printf("Teslim Edilmemiş Kargolar (Teslim Süresine Göre):\n");
    for (int i = 0; i < pq.size; i++) {
        printf("- Kargo ID: %d, Tarih: %s, Süre: %d\n",
               pq.kargolar[i]->gonderiID, pq.kargolar[i]->tarih, pq.kargolar[i]->teslimSuresi);
    }
}

// Ağaç İşlemleri
TreeNode* createTreeNode(int cityID, char *cityName) {
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    newNode->cityID = cityID;
    strcpy(newNode->cityName, cityName);
    newNode->childCount = 0;
    return newNode;
}

void addCity(TreeNode *parent, int cityID, char *cityName) {
    if (parent->childCount >= 10) {
        printf("Maksimum alt şehir sayısına ulaşıldı.\n");
        return;
    }
    TreeNode *newCity = createTreeNode(cityID, cityName);
    parent->children[parent->childCount++] = newCity;
}

void printTree(TreeNode *node, int depth) {
    if (node == NULL) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("- %s (ID: %d)\n", node->cityName, node->cityID);
    for (int i = 0; i < node->childCount; i++) {
        printTree(node->children[i], depth + 1);
    }
}

int calculateTreeDepth(TreeNode *node) {
    if (node == NULL) return 0;
    int maxDepth = 0;
    for (int i = 0; i < node->childCount; i++) {
        int depth = calculateTreeDepth(node->children[i]);
        if (depth > maxDepth) maxDepth = depth;
    }
    return maxDepth + 1;
}

int calculateShortestDeliveryTime(TreeNode *node, int currentDepth) {
    if (node == NULL) return 0;
    if (node->childCount == 0) return currentDepth;
    int minTime = 1e9;
    for (int i = 0; i < node->childCount; i++) {
        int time = calculateShortestDeliveryTime(node->children[i], currentDepth + 1);
        if (time < minTime) minTime = time;
    }
    return minTime;
}

int main() {
    int secim, musteriID, gonderiID, teslimSuresi, cityID, parentCityID;
    char tarih[20], teslimDurumu[50], isim[50], soyisim[50], cityName[50];

    root = createTreeNode(0, "Merkez");

    do {
        printf("\n=== Online Kargo Takip Sistemi ===\n");
        printf("1. Müşteri Ekle\n");
        printf("2. Kargo Gönderimi Ekle\n");
        printf("3. Gönderim Geçmişi Sorgula\n");
        printf("4. Teslim Süreşine Göre Öncelikli Kargo İşle\n");
        printf("5. Teslim Edilmemiş Kargoları Sırala\n");
        printf("6. Şehir Ekle\n");
        printf("7. Ağaç Yapısını Yazdır\n");
        printf("8. Ağacın Derinliğini Hesapla\n");
        printf("9. En Kısa Teslimat Süreşini Hesapla\n");
        printf("10. Teslim Edilmiş Kargoları Listele ve Ara\n");
        printf("11. Çıkış\n");
        printf("Seçiminizi Yapın: ");
        scanf("%d", &secim);
        getchar();

        switch (secim) {
            case 1:
                printf("Müşteri ID: "); scanf("%d", &musteriID); getchar();
                printf("İsim: "); fgets(isim, sizeof(isim), stdin);
                isim[strcspn(isim, "\n")] = 0;
                printf("Soyisim: "); fgets(soyisim, sizeof(soyisim), stdin);
                soyisim[strcspn(soyisim, "\n")] = 0;
                musteriEkle(musteriID, isim, soyisim);
                break;
            case 2:
                printf("Müşteri ID: "); scanf("%d", &musteriID); getchar();
                printf("Gönderi ID: "); scanf("%d", &gonderiID); getchar();
                printf("Tarih: "); fgets(tarih, sizeof(tarih), stdin);
                tarih[strcspn(tarih, "\n")] = 0;
                printf("Teslim Durumu (Teslim Edildi / Teslim Edilmedi): ");
                fgets(teslimDurumu, sizeof(teslimDurumu), stdin);
                teslimDurumu[strcspn(teslimDurumu, "\n")] = 0;
                printf("Teslim Süre: "); scanf("%d", &teslimSuresi); getchar();
                kargoEkle(musteriID, gonderiID, tarih, teslimDurumu, teslimSuresi);
                break;
            case 3:
                printf("Müşteri ID: "); scanf("%d", &musteriID); getchar();
                gonderimGecmisiSorgula(musteriID);
                break;
            case 4:
                printf("\n--- Öncelikli Kargo İşle ---\n");
                priorityQueueIsle(&pq);
                break;
            case 5:
                printf("\n--- Teslim Edilmemiş Kargolar ---\n");
                teslimEdilmemisleriSirala();
                break;
            case 6:
                printf("Şehir ID: "); scanf("%d", &cityID); getchar();
                printf("Şehir Adı: "); fgets(cityName, sizeof(cityName), stdin);
                cityName[strcspn(cityName, "\n")] = 0;
                if (root->childCount == 0) {
                    addCity(root, cityID, cityName);
            }   else {
                    printf("Ebeveyn Şehir ID: "); scanf("%d", &parentCityID); getchar();
                    TreeNode *parent = root;
                    for (int i = 0; i < root->childCount; i++) {
                        if (root->children[i]->cityID == parentCityID) {
                            parent = root->children[i];
                            break;
                }   
            }
            addCity(parent, cityID, cityName);
        }
        break;
            case 7:
                printf("\n--- Ağaç Yapısı ---\n");
                printTree(root, 0);
                break;
            case 8:
                printf("Ağacın Derinliği: %d\n", calculateTreeDepth(root));
            break;
            case 9:
                printf("En Kısa Teslimat Süresi: %d\n", calculateShortestDeliveryTime(root, 0));
                break;
            case 10:
                teslimEdilmisKargolariListeleVeAra();
                break;
            case 11:
                printf("Çıkış yapılıyor...\n");
                break;
            default:
                printf("Geçersiz seçim!\n");
    }
}   while (secim != 11);

    return 0;
}

