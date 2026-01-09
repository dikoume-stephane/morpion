import os
import sys
import shutil
import subprocess

print("=== demarrage du script Morpion ===")

COMPILER = "g++"
INCLUSION = "-Iinclude"
SOURCE_DIRS = ["src", "libs/imgui"]
BUILD_DIR = "build"
EXECUTABLE = os.path.join(BUILD_DIR, "morpion.exe")

# On définit nos flags de base
CFLAGS = ["-std=c++17", "-Wall"]
LDFLAGS = []

def build_et_run():
    if not os.path.exists(BUILD_DIR): os.makedirs(BUILD_DIR)
    
    configurer_sdl()
    tous_objets = []
    fichiers_a_compiler = []

    # On parcourt chaque dossier de la liste blanche
    for dossier in SOURCE_DIRS:
        # On vérifie si le dossier existe pour éviter que le script ne plante 🛑
        if os.path.exists(dossier):
            # os.walk va descendre dans tous les sous-dossiers (ex: src/core)
            for racine, dossiers, fichiers in os.walk(dossier):
                for fichier in fichiers:
                    # On ne garde que les fichiers qui se terminent par .cpp
                    if fichier.endswith(".cpp"):
                        # On crée le chemin complet : "src/core/game_engine.cpp"
                        chemin_complet = os.path.join(racine, fichier)
                        obj = os.path.join(BUILD_DIR, fichier.replace(".cpp", ".o"))
                        tous_objets.append(obj)
                        if doit_compiler(chemin_complet, obj):
                            fichiers_a_compiler.append((chemin_complet, obj))
                            print(f"Trouvé : {chemin_complet}")

    # Petit affichage pour vérifier le nombre de fichier trouvés
    print(f"✅ {len(fichiers_a_compiler)} fichiers source trouvés.")

    # 2. Compiler les fichiers modifiés
    for src, obj in fichiers_a_compiler:
        print(f"🔨 Compilation : {src}")
        res = subprocess.run([COMPILER, INCLUSION, "-c", src, "-o", obj] + CFLAGS)
        if res.returncode != 0:
            print("💥 Erreur de compilation.")
            sys.exit(1)

    # 3. Linking final
    print("🔗 Création de l'exécutable...")
    res = subprocess.run([COMPILER] + [INCLUSION] + tous_objets + ["-o", EXECUTABLE] + LDFLAGS)
    if res.returncode == 0:
        deployer_dll()
        print("✅ Terminé ! Lancement du jeu... 🚀")
        # On lance l'exécutable final
        subprocess.run([EXECUTABLE])
    else:
        print("❌ Erreur lors du linking.")
        sys.exit(1)

def doit_compiler(source, objet):
    if not os.path.exists(objet): return True
    return os.path.getmtime(source) > os.path.getmtime(objet)

def configurer_sdl():
    global CFLAGS, LDFLAGS
    # Test rapide pour SDL3 globale
    test_cmd = [COMPILER, "-xc++", "-", "-o", "nul", "-lSDL3"]
    try:
        if subprocess.run(test_cmd, input=b"", capture_output=True).returncode == 0:
            print("🌍 Utilisation de SDL3 Système")
            LDFLAGS += ["-lSDL3", "-lSDL3_image"]
        else:
            print("🏠 Utilisation de SDL3 Locale")
            CFLAGS += ["-Ilibs/SDL3/include"]
            LDFLAGS += ["-Llibs/SDL3", "-lSDL3", "-lSDL3_image"]
    except:
        print("⚠️ Erreur critique : impossible de lancer g++")
        sys.exit(1)

def deployer_dll():
    #Copie les DLL nécessaires dans le dossier build.
    chemin_libs = "libs/SDL3"
    if os.path.exists(chemin_libs):
        for f in os.listdir(chemin_libs):
            if f.endswith(".dll"):
                dest = os.path.join(BUILD_DIR, f)
                if not os.path.exists(dest):
                    shutil.copy(os.path.join(chemin_libs, f), dest)

def nettoyer_projet():
    dossier_build = "build"
    if os.path.exists(dossier_build):
        print(f"🧹 Nettoyage du dossier {dossier_build}...")
        # rmtree = "Remove Tree" (supprime le dossier et tout ce qu'il contient)
        shutil.rmtree(dossier_build)
        print("✨ Projet propre !")
    else:
        print("🤷 Le dossier build n'existe pas encore, rien à nettoyer.")


if __name__ == "__main__":
    # On regarde ce que l'utilisateur a écrit après "build.py"
    if len(sys.argv) > 1:
        commande = sys.argv[1]
        
        if commande == "clean":
            nettoyer_projet()
        elif commande == "run":
            # 1. On compile
            # 2. On link
            # 3. On lance !
            pass # On remplira avec nos appels de fonctions
    else:
        # Par défaut, on fait juste le build
        print("Usage: python build.py [clean|run]")
if __name__ == "__main__":
    build_et_run()