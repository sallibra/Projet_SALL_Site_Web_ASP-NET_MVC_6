using AvisFormation.WebUi.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace AvisFormation.WebUi.Controllers
{
    public class TotoController : Controller
    {
        // GET: Toto
        public ActionResult Index()
        {
            var vm = new MyViewModel();
            vm.Message = "mon message Ibraima dynamique";
            
            return View(vm);
        }
    }
}