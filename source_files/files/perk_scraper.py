import requests
from bs4 import BeautifulSoup
from unidecode import unidecode
import csv

URL = "https://deadbydaylight.wiki.gg/wiki/Perks#Survivor_Perks_(152)"
SURVIVOR = 0
KILLER = 1

# takes a filepath to a perks list file and returns a dict containing the 
# keys of all perk names paired with the value of their random-enabled attribute 
def rand_enabled_info(filepath: str) -> dict[str, str]:
    ret_dict = {}

    with open(filepath, mode="r") as file:
        csv_reader = csv.DictReader(file)
        for line in csv_reader:
            ret_dict[line["perk-name"]] = line["random-enabled"]

    return ret_dict
        

def main() -> None:
    # store information about which perks are enabled for randomisation
    # to preserve settings
    surv_rand_enabled = rand_enabled_info("./survivor_perks.csv")
    killer_rand_enabled = rand_enabled_info("./killer_perks.csv")
    settings = [surv_rand_enabled, killer_rand_enabled]


    # get page contents
    print("requesting information from: 'deadbydaylight.wiki.gg'...")
    try:
        page = requests.get(URL)
    except:
        print("failed to retrieve website information, please make sure you are connected to the internet\n" \
        "perk list has not been updated")
        return
    
    soup = BeautifulSoup(page.content, "html.parser")

    # search page
    results = soup.find(id="mw-content-text")
    for character_type, table in enumerate(results.find_all("table")[1:3]):  # only 2nd and 3rd table contain perks
        # open relevant file
        if character_type == SURVIVOR:
            file = open("./survivor_perks.csv", "w")
        elif character_type == KILLER:
            file = open("./killer_perks.csv", "w")
        # rewrite csv file header
        file.write("perk-name,character,random-enabled\n")
        
        # find info for each individual perk
        for i, row in enumerate(table.find_all("tr")[1:]):  #[1:] as first header is the top banner
            headers = row.find_all("th")
            # make sure website perk table isnt down
            try:
                perk_name = unidecode(headers[1].text.strip().replace(" ", "-").lower())
                character_name = unidecode(headers[2].text.strip().replace(".", "").replace(" ", "-").lower())
            except:
                print("failed to retrieve perk information. The perk table may be currently unavailable, " \
                "please try again later\nperk list has not been updated")
                return
            # write to file
            if perk_name in settings[character_type]:
                file.write(f"{perk_name},{character_name},{settings[character_type][perk_name]}\n")
            else:
                file.write(f"{perk_name},{character_name},true\n")
            print(f"{i}: {perk_name}, {character_name}")
        file.close()

    print("perks updated successfully")
    return

if __name__ == "__main__":
    main()

