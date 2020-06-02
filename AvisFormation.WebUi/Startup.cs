using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(AvisFormation.WebUi.Startup))]
namespace AvisFormation.WebUi
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);
        }
    }
}
