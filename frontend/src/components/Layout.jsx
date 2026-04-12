import { Outlet, Link } from 'react-router-dom';
import { LogOut, Cloud, QrCode } from 'lucide-react';

export default function Layout({ onLogout }) {
  return (
    <div className="min-h-screen bg-gray-50 flex flex-col">
      <nav className="bg-indigo-600 border-b border-indigo-700">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex justify-between h-16">
            <div className="flex">
              <Link to="/" className="flex-shrink-0 flex items-center text-white text-xl font-bold gap-2">
                <Cloud size={24} />
                Personal Cloud
              </Link>
              <div className="ml-6 flex items-center space-x-4">
                 <Link to="/" className="text-indigo-100 hover:text-white px-3 py-2 rounded-md text-sm font-medium">Files</Link>
                 <Link to="/pairing" className="text-indigo-100 hover:text-white px-3 py-2 rounded-md text-sm font-medium flex items-center gap-1"><QrCode size={16}/> Pair Mobile App</Link>
              </div>
            </div>
            <div className="flex items-center">
              <button
                onClick={onLogout}
                className="text-indigo-100 hover:text-white p-2 rounded-md flex items-center gap-2"
              >
                <LogOut size={20} />
                <span className="text-sm font-medium">Logout</span>
              </button>
            </div>
          </div>
        </div>
      </nav>

      <main className="flex-1 w-full max-w-7xl mx-auto py-6 sm:px-6 lg:px-8">
        <Outlet />
      </main>
    </div>
  );
}
