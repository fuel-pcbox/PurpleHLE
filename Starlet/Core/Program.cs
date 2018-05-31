using System;

namespace Starlet
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Initializing Starlet [PurpleEmu]");
            Console.WriteLine("Version: {0} Build Type: {1}", Core.Settings.Version, Core.Settings.releaseType);
            if(Core.Settings.releaseType == "Beta")
            {
                //warn the poor fucker that all hell is about to break loose
                Console.WriteLine("It appears that you are running a beta version!");
                Console.WriteLine("A lot of stuff will probably break, or be broken at the end of this.");
                Console.WriteLine("Bug reports welcome at {0}", Core.Settings.gitUrl);
            }
        }
    }
}
