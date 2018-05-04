//{Temp(°C), Min(min), IodTest, MashIn}
mashStep mashSteps[] = {{50, 15, false, true}, {68, 60, true, false}, {78, 15, false, false}, };

// {Description, Min}
boilStep boilSteps[] = {{ "H1" , 60}, { "H2+Se+Al" , 10}, { "H3" , 5}, { "H4" , 0}, };
unsigned totalBoilMinutes = 60;

float mash_water_weight = 25;
float mash_malts_weight = 5;

